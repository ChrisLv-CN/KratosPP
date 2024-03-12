#include <exception>
#include <Windows.h>

#include <TechnoClass.h>
#include <UnitClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/TechnoType/TurretAngle.h>
#include <Ext/TechnoType/DisguiseData.h>

#pragma region Unit Deploy
DEFINE_HOOK(0x6FF923, TechnoClass_Fire_FireOnce, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (pTechno->CurrentMission == Mission::Unload)
	{
		pTechno->QueueMission(Mission::Stop, true);
	}
	return 0;
}

DEFINE_HOOK(0x739C7A, UnitClass_Deployed, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		status->OnUpdate_DeployToTransform();
	}
	return 0;
}
#pragma endregion

#pragma region FLH free
DEFINE_HOOK(0x6F3B5C, UnitClassClass_GetFLH_UnbindTurret, 0x6)
{
	enum { turretMitrix = 0x6F3C1A, nextCheck = 0x6F3B62, takeOver = 0x6F3C52 };
	GET(TechnoClass*, pTechno, EBX);
	if (pTechno)
	{
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
		{
			GET_STACK(int, weaponIdx, 0xE0);
			status->FLHIndex = weaponIdx;
			if (pTechno->HasTurret() && status->IsFLHOnBody(weaponIdx))
			{
				Matrix3D matrix = GetMatrix3D(pTechno);
				R->Stack(0x48, matrix);
				return takeOver;
			}
		}
		return nextCheck;
	}
	return turretMitrix;
}

DEFINE_HOOK(0x6F3D2F, UnitClassClass_GetFLH_OnTarget, 0x5)
{
	GET(TechnoClass*, pTechno, EBX);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->IsFLHOnTarget())
	{
		GET(CoordStruct*, pRenderCoord, EAX);
		*pRenderCoord = pTechno->Target->GetCoords();
	}
	return 0;
}
#pragma endregion

#pragma region Unit explosion anims
DEFINE_HOOK(0x738749, UnitClass_Destroy_Explosion_Remap, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EAX);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	return 0;
}

// Take over to Create DestroyAnim Anim
DEFINE_HOOK(0x738801, UnitClass_Destroy_DestroyAnim_Remap, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->PlayDestroyAnims())
		{

			return 0x73887E;
		}
	}
	return 0;
}
#pragma endregion

#pragma region UnitClass Disguise
DEFINE_HOOK(0x6F4222, TechnoClass_Init_PermaDisguise, 0x7)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(HouseClass*, pHouse, EAX);
	bool custom = false;
	ObjectTypeClass* pDisguise = nullptr;
	switch (pTechno->WhatAmI())
	{
	case AbstractType::Unit:
	{
		// 伪装成自己
		pDisguise = pTechno->GetType();
		// 检查微观设置
		DisguiseData* unitData = INI::GetConfig<DisguiseData>(INI::Rules, pDisguise->ID)->Data;
		if (IsNotNone(unitData->DefaultUnitDisguise))
		{
			UnitTypeClass* pType = UnitTypeClass::Find(unitData->DefaultUnitDisguise.c_str());
			if (pType)
			{
				// 伪装成指定的坦克
				pDisguise = static_cast<ObjectTypeClass*>(pType);
				custom = true;
			}
		}
		if (!custom)
		{
			// 阵营所属的伪装坦克
			int sideIndex = pHouse->SideIndex;
			if (sideIndex >= 0 && SideClass::Array->Count > sideIndex)
			{
				std::string sideId = SideClass::Array->GetItem(sideIndex)->ID;
				DisguiseData* sideData = INI::GetConfig<DisguiseData>(INI::Rules, sideId.c_str())->Data;
				if (IsNotNone(sideData->DefaultUnitDisguise))
				{
					UnitTypeClass* pType = UnitTypeClass::Find(sideData->DefaultUnitDisguise.c_str());
					if (pType)
					{
						// 伪装成指定的坦克
						pDisguise = static_cast<ObjectTypeClass*>(pType);
						custom = true;
					}
				}
			}
		}
		custom = true;
		break;
	}
	case AbstractType::Infantry:
	{
		// 阵营设置，Ares接管，不理会，只管微观设置
		DisguiseData* infData = INI::GetConfig<DisguiseData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
		if (IsNotNone(infData->DefaultDisguise))
		{
			InfantryTypeClass* pType = InfantryTypeClass::Find(infData->DefaultDisguise.c_str());
			if (pType)
			{
				// 伪装成指定的步兵
				pDisguise = static_cast<ObjectTypeClass*>(pType);
				custom = true;
			}
		}
		break;
	}
	}
	if (custom && pDisguise)
	{
		pTechno->Disguised = true;
		pTechno->DisguisedAsHouse = pHouse;
		pTechno->Disguise = pDisguise;
		return 0x6F4277;
	}
	return 0;
}

DEFINE_HOOK(0x7466D8, UnitClass_Set_Disguise, 0xA)
{
	GET(AbstractClass*, pTarget, ESI);
	if (static_cast<ObjectClass*>(pTarget)->IsDisguised())
	{
		// 伪装成目标单位的伪装
		return 0x7466E6;
	}
	else if (pTarget->WhatAmI() == AbstractType::Unit)
	{
		// 我自己来
		TechnoClass* pTargetTechno = static_cast<TechnoClass*>(pTarget);
		GET(TechnoClass*, pTechno, EDI);
		pTechno->Disguise = pTargetTechno->GetType();
		R->EAX(reinterpret_cast<unsigned int>(pTarget->GetOwningHouse()));
		return 0x746704;
	}
	return 0x746712;
}

DEFINE_HOOK(0x746AFF, UnitClass_Disguise_Update_MoveToClear, 0xA)
{
	GET(TechnoClass*, pTechno, ESI);
	ObjectTypeClass* pDisguise = pTechno->Disguise;
	// Don't clear UnitType
	if (!pDisguise || pDisguise->WhatAmI() != AbstractType::UnitType)
	{
		pTechno->ClearDisguise();
	}
	return 0x746A9C;
}

DEFINE_HOOK(0x746B6D, UnitClass_Disguise_FullName, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);
	if (pTechno->IsDisguised())
	{
		HouseClass* pPlayer = HouseClass::CurrentPlayer;
		HouseClass* pHouse = pTechno->Owner;
		if (!pHouse
			|| (pHouse != pPlayer && !pHouse->IsAlliedWith(pPlayer)))
		{
			// 显示伪装对象的名字
			return 0x746B48;
		}
	}
	if (pTechno->GetTechnoType()->HasMultipleTurrets())
	{
		// 显示乘客的名字
		return 0x746B7A;
	}
	// 显示单位自己的名字
	return 0x746C7A;
}


DEFINE_HOOK(0x736A26, UnitClass_Rotation_SetTurretFacingToTarget_Skip, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);

	TurretAngle* status = nullptr;
	if (TryGetScript<TechnoExt, TurretAngle>(pTechno, status) && status->LockTurret)
	{
		// 不允许转炮塔
		R->EDX(&status->LockTurretDir);
	}
	return 0;
}

DEFINE_HOOK(0x736BCA, UnitClass_Rotation_SetTurretFacing_NoTargetAndStanding, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TurretAngle* status = nullptr;
	if (TryGetScript<TechnoExt, TurretAngle>(pTechno, status) && status->ChangeDefaultDir)
	{
		// 炮塔转到指定角度，非车体前方
		pTechno->SecondaryFacing.SetDesired(status->LockTurretDir);
		return 0x736BE2;
	}
	return 0;
}

DEFINE_HOOK(0x736BBB, UnitClass_Rotation_SetTurretFacing_NoTargetAndMoving, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);

	TurretAngle* status = nullptr;
	if (TryGetScript<TechnoExt, TurretAngle>(pTechno, status) && status->LockTurret)
	{
		// 炮塔转到指定角度，非车体前方
		pTechno->SecondaryFacing.SetDesired(status->LockTurretDir);
		return 0x736BE2;
	}
	return 0;
}

DEFINE_HOOK(0x73C71D, UnitClass_DrawSHP_FacingDir, 0x6)
{
	GET(TechnoClass*, pTechno, EBP);
	if (pTechno->IsDisguised() && !pTechno->IsClearlyVisibleTo(HouseClass::CurrentPlayer))
	{
		// WWSB 自己算起始帧
		UnitTypeClass* pTargetType = static_cast<UnitTypeClass*>(pTechno->GetDisguise(true));
		if (pTargetType->WhatAmI() == AbstractType::UnitType)
		{
			int facing = pTargetType->Facings;
			// 0的方向是游戏中的北方，是↗，素材0帧是朝向0点，是↑
			int index = Dir2FrameIndex(pTechno->PrimaryFacing.Current(), facing);
			// Logger.Log($"{Game.CurrentFrame} OOXX dirIndex = {index}, facing = {facing}, walk = {pTargetType->WalkFrames}, fire = {pTargetType->FiringFrames}, {R->EDX} {R->EBX} x {R->ESI}");
			// EDX是播放的帧序号
			GET(int, frameOffset, EDX);
			if (frameOffset == 0)
			{
				frameOffset += index;
				// 站立状态
				R->EDX(frameOffset);
			}
			else
			{
				// 移动状态
				// ???, UnitTypeClass.WalkFrames拿到的不是WalkFrames
				int walkFrames = INI::GetSection(INI::Art, pTargetType->ID)->Get("WalkFrames", 1);
				frameOffset += index * walkFrames + pTargetType->StartWalkFrame;
				R->EDX(frameOffset);
			}
		}
	}
	return 0;
}

DEFINE_HOOK(0x73C655, UnitClass_DrawSHP_TechnoType, 0x6)
{
	GET(TechnoClass*, pTechno, EBP);
	ObjectTypeClass* pTargetType = pTechno->GetDisguise(true);
	if (pTechno->IsDisguised() && !pTechno->IsClearlyVisibleTo(HouseClass::CurrentPlayer)
		&& pTargetType->WhatAmI() == AbstractType::UnitType)
	{
		R->ECX(reinterpret_cast<unsigned int>(pTargetType));
		return 0x73C65B;
	}
	return 0;
}

DEFINE_HOOK(0x73C69D, UnitClass_DrawSHP_TechnoType2, 0x6)
{
	GET(TechnoClass*, pTechno, EBP);
	ObjectTypeClass* pTargetType = pTechno->GetDisguise(true);
	if (pTechno->IsDisguised() && !pTechno->IsClearlyVisibleTo(HouseClass::CurrentPlayer)
		&& pTargetType->WhatAmI() == AbstractType::UnitType)
	{
		R->ECX(reinterpret_cast<unsigned int>(pTargetType));
		return 0x73C6A3;
	}
	// Stand is Moving
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->StandIsMoving)
	{
		return 0x73C702;
	}
	return 0;
}

DEFINE_HOOK(0x73C702, UnitClass_DrawSHP_TechnoType3, 0x6)
{
	GET(TechnoClass*, pTechno, EBP);
	ObjectTypeClass* pTargetType = pTechno->GetDisguise(true);
	if (pTechno->IsDisguised() && !pTechno->IsClearlyVisibleTo(HouseClass::CurrentPlayer)
		&& pTargetType->WhatAmI() == AbstractType::UnitType)
	{
		R->ECX(reinterpret_cast<unsigned int>(pTargetType));
		return 0x73C708;
	}
	return 0;
}

DEFINE_HOOK(0x73C725, UnitClass_DrawSHP_HasTurret, 0x6)
{
	GET(TechnoClass*, pTechno, EBP);
	if (pTechno->IsDisguised() && !pTechno->IsClearlyVisibleTo(HouseClass::CurrentPlayer))
	{
		ObjectTypeClass* pTargetType = pTechno->GetDisguise(true);
		if (pTargetType && !static_cast<TechnoTypeClass*>(pTargetType)->Turret)
		{
			// no turret
			return 0x73CE0D;
		}
	}
	return 0;
}

DEFINE_HOOK_AGAIN(0x73B765, UnitClass_DrawVoxel_TurretFacing, 0x5)
DEFINE_HOOK_AGAIN(0x73BA78, UnitClass_DrawVoxel_TurretFacing, 0x6)
DEFINE_HOOK_AGAIN(0x73BD8B, UnitClass_DrawVoxel_TurretFacing, 0x5)
DEFINE_HOOK(0x73BDA3, UnitClass_DrawVoxel_TurretFacing, 0x5)
{
	GET(TechnoClass*, pTechno, EBP);
	if (!pTechno->GetTechnoType()->Turret && pTechno->IsDisguised() && !pTechno->IsClearlyVisibleTo(HouseClass::CurrentPlayer))
	{
		// 本体没有炮塔
		ObjectTypeClass* pTargetType = pTechno->GetDisguise(true);
		if (pTargetType && static_cast<TechnoTypeClass*>(pTargetType)->Turret)
		{
			// 伪装的对象有炮塔，将身体的朝向赋给炮塔
			GET(DirStruct*, pDir, EAX);
			DirStruct dir = pTechno->PrimaryFacing.Current();
			pDir->SetValue(dir.GetValue());
		}
	}
	return 0;
}

DEFINE_HOOK(0x73B8DB, UnitClass_DrawVoxel_HasChargeTurret, 0x8)
{
	GET(TechnoClass*, pTechno, EBP);
	GET(TechnoTypeClass*, pTechnoType, EBX);
	// 渲染伪装对象
	if (pTechnoType != pTechno->GetTechnoType())
	{
		if (!pTechnoType->HasMultipleTurrets() || pTechnoType->IsGattling)
		{
			// 渲染炮管
			return 0x73B92F;
		}
		else
		{
			// 继续检查
			return 0x73B8FC;
		}
	}
	return 0;
}

// DEFINE_HOOK_AGAIN(0x73BA53, UnitClass_DrawVoxel_HasChargeTurret2, 0x7) // TurretOffset
DEFINE_HOOK(0x73BC1D, UnitClass_DrawVoxel_HasChargeTurret2, 0x7)
{
	GET(TechnoClass*, pTechno, EBP);
	GET(TechnoTypeClass*, pTechnoType, EBX);
	// 渲染伪装对象
	if (pTechnoType != pTechno->GetTechnoType())
	{
		if (pTechnoType->TurretCount > 0 && !pTechnoType->IsGattling)
		{
			if (pTechno->CurrentTurretNumber < 0)
			{
				R->Stack<int>(0x1C, 0);
			}
			return 0x73BC35;
		}
		else
		{
			return 0x73BD79;
		}
	}
	return 0;
}

DEFINE_HOOK(0x706724, TechnoClass_Draw_VXL_Disguise_Blit_Flags, 0x5)
{
	return 0x706731;
}
#pragma endregion
