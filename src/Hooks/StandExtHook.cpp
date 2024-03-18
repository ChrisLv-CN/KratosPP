#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <AircraftClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>
#include <Extension/WarheadTypeExt.h>

#include <Ext/Common/CommonStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/ObjectType/AttachEffect.h>

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
DEFINE_HOOK(0x704368, TechnoClass_GetZAdjust_Stand, 0x8)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(int, height, EAX);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && !status->MyStandData.IsTrain)
	{
		int zAdjust = TacticalClass::Instance->AdjustForZ(height);
		int offset = status->MyStandData.ZOffset;
		R->ECX(zAdjust + offset);
	}
	return 0;
}

DEFINE_HOOK(0x4DB7F7, FootClass_In_Which_Layer_Stand, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);

	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && !status->MyStandData.IsTrain && status->MyStandData.ZOffset != 0)
	{
		Layer layer = status->MyStandData.DrawLayer;
		if (layer == Layer::None)
		{
			layer = pTechno->IsInAir() ? Layer::Top : Layer::Air;
		}
		// R->ECX((DWORD)layer);
		// return 0x4DB803; //Get Error
	}
	return 0;
}

DEFINE_HOOK(0x54B8E9, JumpjetLocomotionClass_In_Which_Layer_Deviation, 0x6)
{
	GET(TechnoClass*, pTechno, EAX);

	if (pTechno->IsInAir()) {
		AttachEffect* aeManager = nullptr;
		if (TryGetAEManager<TechnoExt>(pTechno, aeManager) && aeManager->HasStand())
		{
			// Override JumpjetHeight / CruiseHeight check so it always results in 3 / Layer::Air.
			R->EDX(INT32_MAX);
			return 0x54B96B;
		}
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


