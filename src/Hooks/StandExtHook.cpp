﻿#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <AircraftClass.h>
#include <DriveLocomotionClass.h>
#include <JumpjetLocomotionClass.h>
#include <ShipLocomotionClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>
#include <Extension/WarheadTypeExt.h>

#include <Ext/Common/CommonStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/ObjectType/AttachEffect.h>

DEFINE_HOOK(0x6FA2A2, TechnoClass_Update_DrawBehind, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (AmIStand(pTechno))
	{
		// skip draw behind anim
		return 0x6FA2DA;
	}
	return 0;
}

// AI的替身在攻击友军时会强制取消目标
DEFINE_HOOK(0x6FA45D, TechnoClass_Update_NotHuman_ClearTarget_Stand, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (CombatDamage::Data()->AllowAIAttackFriendlies || AmIStand(pTechno))
	{
		return 0x6FA472;
	}
	return 0;
}

DEFINE_HOOK(0x5F6B90, ObjectClass_InAir_SkipCheckIsOnMap, 0x5)
{
	GET(ObjectClass*, pObject, ECX);
	TechnoClass* pTechno = nullptr;
	if (CastToTechno(pObject, pTechno) && AmIStand(pTechno))
	{
		return 0x5F6B97;
	}
	return 0;
}

// Stand can't set destination
// BlackHole's victim can't set destination
// Jumping unit can't set destination
DEFINE_HOOK(0x4D94B0, FootClass_SetDestination_Stand, 0x5)
{
	GET(TechnoClass*, pTechno, ECX);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && (status->AmIStand() || status->CaptureByBlackHole || status->Jumping || status->Freezing))
	{
		// 跳过目的地设置
		return 0x4D9711;
	}
	return 0;
}

// Stand support Radar
DEFINE_HOOK(0x508EC6, HouseClass_RadarUpdate_Stand, 0x5)
{
	GET(TechnoClass*, pTechno, EAX);
	if (pTechno->IsOnMap || AmIStand(pTechno))
	{
		return 0x508ECD;
	}
	return 0x508F08;
}

// Stand support SpySat
DEFINE_HOOK(0x508F9B, HouseClass_SpySatUpdate_Stand, 0x5)
{
	GET(TechnoClass*, pTechno, ECX);
	if (pTechno->IsOnMap || AmIStand(pTechno))
	{
		return 0x508FA2;
	}
	return 0x508FF6;
}

// 替身需要显示在上层时，修改了渲染的层，导致单位在试图攻击替身时，需要武器具备AA
DEFINE_HOOK(0x6FC749, TechnoClass_CanFire_WhichLayer_Stand, 0x5)
{
	GET(Layer, layer, EAX);
	enum { inAir = 0x6FC74E, onGround = 0x6FC762 };
	if (layer != Layer::Ground)
	{
		try
		{
			GET_STACK(AbstractClass*, pTarget, 0x20 - (-0x4));
			TechnoClass* pTechno = nullptr;
			if (CastToTechno(pTarget, pTechno) && AmIStand(pTechno))
			{
				if (pTechno->IsInAir())
				{
					return inAir;
				}
				return onGround;
			}
		}
		catch (const std::exception& e)
		{
			Debug::Log("TechnoClass_CanFire_WhichLayer_Stand throw exception: %s \n", e.what());
		}
		return inAir;
	}
	return onGround;
}

DEFINE_HOOK(0x6FF66C, TechnoClass_Fire_DecreaseAmmo, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand()
		&& status->pMyMaster && status->MyStandData.UseMasterAmmo)
	{
		int ammo = status->pMyMaster->Ammo;
		if (ammo > 0)
		{
			status->pMyMaster->Ammo--;
			status->pMyMaster->StartReloading();
		}
	}
	return 0;
}


#pragma region Pick Stand as Target

DEFINE_HOOK(0x6FCDBE, TechnoClass_SetTarget_Stand, 0x6)
{
	GET(AbstractClass*, pTarget, EDI);
	if (pTarget)
	{
		TechnoClass* pTechno = nullptr;
		TechnoStatus* status = nullptr;
		if (CastToTechno(pTarget, pTechno) && TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand()
			&& (status->MyStandData.Immune || !pTechno->GetTechnoType() || pTechno->GetTechnoType()->Insignificant || IsImmune(pTechno))
			)
		{
			// 替身处于无敌状态，目标设置为替身使者
			R->EDI(status->pMyMaster);
		}
	}
	return 0;
}

//6F9256 6 AircraftType IsOnMap ? 6F925C : 6F9377
DEFINE_HOOK(0x4D9947, FootClass_Greatest_Threat_GetTarget, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AbstractClass*, pTarget, EAX);
	if (!pTarget && CombatDamage::Data()->AllowAutoPickStandAsTarget)
	{
		// no target, extend find Stand
		// 搜索符合条件的替身
		// Logger.Log($"{Game.CurrentFrame}, {pTechno}自动搜索目标, 替身列表里有 {TechnoStatusScript.StandArray.Count()} 个记录");
		for (auto stand : TechnoExt::StandArray)
		{
			TechnoClass* pStand = stand.first;
			if (!stand.second.Immune && pStand->GetTechnoType() && !pStand->GetTechnoType()->Insignificant && !IsImmune(pStand))
			{
				// 是否可以攻击，并获得攻击使用的武器序号
				pTarget = pStand;
				int weaponIdx = pTechno->SelectWeapon(pTarget);
				bool canFire = CanAttack(pTechno, pTarget, true, weaponIdx);
				if (canFire)
				{
					// 检查所属
					int damage = pTechno->CombatDamage(weaponIdx);
					if (damage < 0)
					{
						// 维修武器
						canFire = pTechno->Owner->IsAlliedWith(pStand->Owner) && pStand->Health < pStand->GetTechnoType()->Strength;
					}
					else
					{
						// 只允许攻击敌人
						canFire = !pTechno->Owner->IsAlliedWith(pStand->Owner);
						if (canFire)
						{
							// 检查平民
							if (IsCivilian(pStand->Owner))
							{
								// Ares 的平民敌对目标
								canFire = INI::GetSection(INI::Rules, pStand->GetTechnoType()->ID)->Get("CivilianEnemy", false);
								// Ares 的反击平民
								TechnoClass* pStandTarget = nullptr;
								if (!canFire && AutoRepel(pTechno->Owner) && CastToTechno(pStand->Target, pStandTarget))
								{
									canFire = pTechno->Owner->IsAlliedWith(pStandTarget->Owner);
								}
							}
						}
					}
				}
				if (canFire)
				{
					// Pick this Stand as Target
					R->EAX(pStand);
					break;
				}
			}
		}

	}
	return 0;
}

#pragma endregion

#pragma region Stand Drawing
// WWSB只排序在地面的layer，不排序在空中的layer
DEFINE_HOOK(0x55DBC3, MainLoop_ShortLayerClass, 0x5)
{
	DisplayClass::GetLayer(Layer::Air)->Sort();
	return 0;
}

DEFINE_HOOK(0x4DA87A, FootClass_Update_UpdateLayer, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (pTechno->IsAlive && !pTechno->InLimbo)
	{
		if (pTechno->InWhichLayer() != pTechno->LastLayer)
		{
			DisplayClass::Instance->Submit(pTechno);
		}
	}
	return 0;
}

namespace StandZAdjust
{
	TechnoClass* pSkip = nullptr;
	TechnoClass* pStand = nullptr;
	int zAdjust = -1;
}

DEFINE_HOOK(0x4DB091, FootClass_GetZAdjustment, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (pTechno == StandZAdjust::pStand)
	{
		R->EDI(0);
		R->Stack(0x8, 0);
		R->EAX(StandZAdjust::zAdjust);
	}
	else if (pTechno == StandZAdjust::pSkip)
	{
		// 什么都不做
	}
	else
	{
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && status->pMyMaster && !status->MyStandData.IsTrain)
		{
			// 从JOJO身上获取ZAdjust
			TechnoClass* pMaster = status->pMyMaster;
			int z = pMaster->GetZAdjustment();
			if (BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(pMaster))
			{
				if (status->MyStandData.ZOffset < 0)
				{
					z += 3;
				}
				else
				{
					// 加上自身的zAdjust
					int zz = pTechno->GetZ();
					int zzz = -TacticalClass::Instance->AdjustForZ(zz);
					z += zzz;
					// 加上建筑动画的zAdjust
					BuildingTypeClass* pType = pBuilding->Type;
					int bZ = 0;
					for (int i = 0; i < 0x15; i++)
					{
						int zAdjust = pType->BuildingAnim[i].ZAdjust;
						if (bZ > zAdjust)
						{
							bZ = zAdjust;
						}
					}
					// 再加上一层，每一层高度是15
					z += bZ - 14;
				}
			}

			StandZAdjust::pSkip = nullptr;
			StandZAdjust::pStand = pTechno;
			StandZAdjust::zAdjust = z;
			R->EDI(0);
			R->Stack(0x8, 0);
			R->EAX(z);
		}
		else
		{
			StandZAdjust::pSkip = pTechno;
			StandZAdjust::pStand = nullptr;
			StandZAdjust::zAdjust = -1;
		}
	}
	return 0;
}

namespace StandYSort
{
	ObjectClass* pSkip = nullptr;
	ObjectClass* pStand = nullptr;
	int X = 0;
	int Y = 0;
}

// Whether to render in the upper or lower layers is controlled by sorting DesplayClass::LayerClass[Layer].
// When Layer==Ground, use the YSort function for sorting,
// and the return value is the X + Y of the rendering coordinates.
DEFINE_HOOK(0x5F6BF7, ObjectClass_GetYSort, 0x5)
{
	GET(ObjectClass*, pObject, ESI);
	GET(int*, x, EAX);
	int* y = (int*)(R->EDI() + 4);

	if (pObject == StandYSort::pStand)
	{
		*x = StandYSort::X;
		*y = StandYSort::Y;
	}
	else if (pObject == StandYSort::pSkip)
	{
		// 什么都不做
	}
	else if (TechnoClass* pTechno = dynamic_cast<TechnoClass*>(pObject))
	{
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt, TechnoStatus>(pTechno, status) && status->AmIStand() && status->pMyMaster && !status->MyStandData.IsTrain)
		{
			// 替身从Master身上获取渲染坐标
			CoordStruct r = status->pMyMaster->GetRenderCoords();
			int offset = status->MyStandData.ZOffset;
			if (offset >= 0)
			{
				if (BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(status->pMyMaster))
				{
					BuildingTypeClass* pType = pBuilding->Type;
					int bY = 0;
					for (int i = 0; i < 0x15; i++)
					{
						int ySort = pType->BuildingAnim[i].YSort;
						if (bY < ySort)
						{
							bY = ySort;
						}
					}
					offset += bY + 16;
				}
			}
				// 修改返回值
			*x = r.X + offset;
			*y = r.Y + offset;
		}
		StandYSort::pSkip = nullptr;
		StandYSort::pStand = pObject;
		StandYSort::X = *x;
		StandYSort::Y = *y;
	}
	else
	{
		StandYSort::pSkip = pObject;
		StandYSort::pStand = nullptr;
	}
	return 0;
}

namespace StandLayer
{
	TechnoClass* pStand = nullptr;
	Layer layer = Layer::Ground;

	static Layer GetLayer(TechnoClass* pTechno)
	{
		Layer result = Layer::Ground;
		if (pTechno == pStand)
		{
			result = layer;
		}
		else
		{
			result = pTechno->IsInAir() && !pTechno->IsFallingDown ? Layer::Air : Layer::Ground;
			TechnoStatus* status = nullptr;
			if (TryGetStatus<TechnoExt, TechnoStatus>(pTechno, status) && status->AmIStand() && status->pMyMaster && !status->MyStandData.IsTrain)
			{
				// 替身从Master身上获取渲染层
				Layer l = status->pMyMaster->LastLayer;
				if (l != Layer::None)
				{
					result = l;
				}
			}
			pStand = pTechno;
			layer = result;
		}
		return result;
	}
}

DEFINE_HOOK(0x75C7E0, WalkLocomotionClass_In_Which_Layer, 0x5) // WalkLoco
{
	GET(TechnoClass*, pTechno, ESI);
	Layer layer = StandLayer::GetLayer(pTechno);
	R->EAX(layer);
	return 0x75C7E5;
}

DEFINE_HOOK(0x6A3E50, ShipLocomotionClass_In_Which_Layer, 0x5) // ShipLoco
{
	GET(TechnoClass*, pTechno, ESI);
	Layer layer = StandLayer::GetLayer(pTechno);
	R->EAX(layer);
	return 0x6A3E55;
}

DEFINE_HOOK(0x5B19D0, MechLocomotionClass_In_Which_Layer, 0x5) // MechLoco
{
	GET(TechnoClass*, pTechno, ESI);
	Layer layer = StandLayer::GetLayer(pTechno);
	R->EAX(layer);
	return 0x5B19D5;
}

DEFINE_HOOK(0x517100, HoverLocomotionClass_In_Which_Layer, 0x5) // HoverLoco
{
	GET(TechnoClass*, pTechno, ESI);
	Layer layer = StandLayer::GetLayer(pTechno);
	R->EAX(layer);
	return 0x517105;
}

DEFINE_HOOK(0x4B4820, DriveLocomotionClass_In_Which_Layer, 0x5) // DriveLoco
{
	GET(TechnoClass*, pTechno, ESI);
	Layer layer = StandLayer::GetLayer(pTechno);
	R->EAX(layer);
	return 0x4B4825;
}

// 不允许JJ类型，layer设置为top
DEFINE_HOOK(0x54B8E9, JumpjetLocomotionClass_In_Which_Layer_Deviation, 0x6)
{
	GET(TechnoClass*, pTechno, EAX);

	if (pTechno->IsInAir()) {
		// Override JumpjetHeight / CruiseHeight check so it always results in 3 / Layer::Air.
		R->EDX(INT32_MAX);
		return 0x54B96B;
	}
	return 0;
}
#pragma endregion

#pragma region Amin ChronoSparkle

DEFINE_HOOK(0x414C27, AircraftClass_Update_SkipCreateChronoSparkleAnimOnStand, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && status->MyStandData.Immune)
	{
		return 0x414C78;
	}
	return 0;
}

DEFINE_HOOK(0x440406, BuildingClass_Update_SkipCreateChronoSparkleAnimOnStand1, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && status->MyStandData.Immune)
	{
		return 0x44055D;
	}
	return 0;
}

DEFINE_HOOK(0x44050C, BuildingClass_Update_SkipCreateChronoSparkleAnimOnStand2, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && status->MyStandData.Immune)
	{
		return 0x44055D;
	}
	return 0;
}

DEFINE_HOOK(0x51BB17, InfantryClass_Update_SkipCreateChronoSparkleAnimOnStand, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && status->MyStandData.Immune)
	{
		return 0x51BB6E;
	}
	return 0;
}

DEFINE_HOOK(0x736250, UnitClass_Update_SkipCreateChronoSparkleAnimOnStand, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && status->MyStandData.Immune)
	{
		return 0x7362A7;
	}
	return 0;
}

#pragma endregion


