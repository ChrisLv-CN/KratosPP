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
#include <Ext/TechnoType/AircraftAttitude.h>
#include <Ext/TechnoType/AircraftDive.h>
#include <Ext/TechnoType/AircraftGuard.h>

DEFINE_HOOK(0x639DD8, PlanningManager_AllowAircraftsWaypoint, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);
	switch (pTechno->WhatAmI())
	{
	case AbstractType::Infantry:
	case AbstractType::Unit:
		return 0x639DDD;
	case AbstractType::Aircraft:
		if (!pTechno->GetTechnoType()->Spawned)
		{
			return 0x639DDD;
		}
	}
	return 0x639E03;
}

#pragma region DrawShadow
DEFINE_HOOK_AGAIN(0x73C4F8, TechnoClass_DrawShadow, 0x7) // InAir
DEFINE_HOOK_AGAIN(0x73C58E, TechnoClass_DrawShadow, 0x7) // OnGround
DEFINE_HOOK(0x414876, TechnoClass_DrawShadow, 0x7) // Aircraft
{
	GET(TechnoClass*, pTechno, EBP);
	GET(Matrix3D*, pMatrix, EAX);
	if (pTechno->GetTechnoType()->ConsideredAircraft || pTechno->WhatAmI() == AbstractType::Aircraft)
	{
		// 缩放影子
		pMatrix->Scale(AudioVisual::Data()->VoxelShadowScaleInAir);

		// 调整倾斜时影子的纵向比例
		Matrix3D matrix = dynamic_cast<FootClass*>(pTechno)->Locomotor->Draw_Matrix(nullptr);
		double scale = Math::cos(abs(matrix.GetYRotation()));
		pMatrix->ScaleX(static_cast<float>(scale));
	}
	return 0;
}
#pragma endregion


#pragma region Aircraft Attitude

DEFINE_HOOK(0x4CF80D, FlyLocomotionClass_Draw_Matrix, 0x5)
{
	FlyLocomotionClass* pFly = (FlyLocomotionClass*)(R->ESI() - 4);
	if (TechnoClass* pTechno = pFly->LinkedTo)
	{
		AircraftAttitude* attitude = nullptr;
		if (TryGetScript<TechnoExt, AircraftAttitude>(pTechno, attitude) && attitude->PitchAngle != 0)
		{
			GET_STACK(Matrix3D, matrix3D, 0x8);
			matrix3D.RotateY(attitude->PitchAngle);
			R->Stack(0x8, matrix3D);
		}
	}
	return 0;
}

DEFINE_HOOK(0x4CF4C5, FlyLocomotionClass_FlightLevel_ResetA, 0xA)
{
	GET(FlyLocomotionClass*, pFly, EBP);
	int flightLevel = RulesClass::Instance->FlightLevel;
	if (TechnoClass* pTechno = pFly->LinkedTo)
	{
		AircraftDive* dive = nullptr;
		if (TryGetScript<TechnoExt, AircraftDive>(pTechno, dive)
			&& dive->DiveStatus == AircraftDive::AircraftDiveStatus::DIVEING)
		{
			flightLevel = dive->GetAircraftDiveData()->FlightLevel;
		}
	}
	R->EAX(flightLevel);
	return 0x4CF4CF;
}

DEFINE_HOOK(0x4CF3E3, FlyLocomotionClass_FlightLevel_ResetB, 0x9)
{
	GET(FlyLocomotionClass*, pFly, EBP);
	if (TechnoClass* pTechno = pFly->LinkedTo)
	{
		AircraftDive* dive = nullptr;
		if (TryGetScript<TechnoExt, AircraftDive>(pTechno, dive)
			&& dive->DiveStatus == AircraftDive::AircraftDiveStatus::DIVEING)
		{
			return 0x4CF4D2;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4CF3C5, FlyLocomotionClass_4CEFB0, 0x6)
{
	// 调整飞机的朝向，有目标时获取目标的朝向，没有目标时获得默认朝向，此时EAX为0
	// EAX是目标DirStruct的指针
	// ECX是当前Facing的指针
	// ESI是飞机的指针的指针
	GET(DirStruct*, pDirEAX, EAX);
	if (!pDirEAX)
	{
		GET(DirStruct*, pDir, EDX);
		GET(TechnoClass**, ppTechno, ESI);
		TechnoClass* pTechno = *ppTechno;
		// 如果是Spawnd就全程强制执行
		// Mission是Enter的普通飞机就不管
		if (pTechno->IsInAir()
			&& (pTechno->GetTechnoType()->Spawned || pTechno->CurrentMission != Mission::Enter))
		{
			pDir->SetValue(pTechno->SecondaryFacing.Current().GetValue());
		}
	}
	return 0;
}

// Hook address form Otamma
DEFINE_HOOK(0x41B760, IFlyControl_Landing_Direction, 0x6)
{
	GET_STACK(IFlyControl*, pAircraft, 0x4); // IFlyControl*
	int poseDir = RulesClass::Instance->PoseDir;
	AircraftAttitude* attitude = nullptr;
	TechnoClass* pTechno = dynamic_cast<AircraftClass*>(pAircraft);
	if (TryGetScript<TechnoExt, AircraftAttitude>(pTechno, attitude)
		&& attitude->TryGetAirportDir(poseDir))
	{
		R->EAX(poseDir);
		return 0x41B7C1;
	}
	return 0;
}

#pragma endregion

#pragma region Aircraft Guard
DEFINE_HOOK(0x41A697, AircraftClass_Mission_Guard_NoTarget_Enter, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AircraftGuard* fighter = nullptr;
	if (TryGetScript<TechnoExt, AircraftGuard>(pTechno, fighter)
		&& fighter->IsAreaGuardRolling())
	{
		// 不返回机场，而是继续前进直到目的地
		return 0x41A6AC;
	}
	return 0;
}

DEFINE_HOOK(0x41A96C, AircraftClass_Mission_GuardArea_NoTarget_Enter, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AircraftGuard* fighter = nullptr;
	if (TryGetScript<TechnoExt, AircraftGuard>(pTechno, fighter))
	{
		// 不返回机场，而是继续前进直到目的地
		fighter->StartAreaGuard();
		return 0x41A97A;
	}
	return 0;
}

DEFINE_HOOK(0x4CF780, FlyLocomotionClass_Draw_Matrix_Rolling, 0x5)
{
	FlyLocomotionClass* pFly = (FlyLocomotionClass*)(R->ESI() - 4);
	TechnoClass* pTechno = pFly->LinkedTo;
	AircraftGuard* fighter = nullptr;
	if (pTechno && pTechno->GetTechnoType()->RollAngle != 0
		&& TryGetScript<TechnoExt, AircraftGuard>(pTechno, fighter)
		&& fighter->State == AircraftGuard::AircraftGuardStatus::ROLLING)
	{
		// 保持倾斜
		if (fighter->Clockwise)
		{
			return 0x4CF7B0; // 右倾
		}
		else
		{
			return 0x4CF7DF; // 左倾
		}
	}
	return 0;
}

#pragma endregion

DEFINE_HOOK(0x418478, AircraftClass_Mission_Attack_Fire_Imcoming_0, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AbstractClass* pTarget = pTechno->Target;
	int weaponIdx = pTechno->SelectWeapon(pTarget);
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	if (pWeapon && pWeapon->WeaponType && pWeapon->WeaponType->Damage <= 0)
	{
		// skip scatter target cell
		return 0x4184C2;
	}
	return 0;
}

DEFINE_HOOK(0x4186D7, AircraftClass_Mission_Attack_Fire_Imcoming_1, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AbstractClass* pTarget = pTechno->Target;
	int weaponIdx = pTechno->SelectWeapon(pTarget);
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	if (pWeapon && pWeapon->WeaponType && pWeapon->WeaponType->Damage <= 0)
	{
		// skip scatter target cell
		return 0x418720;
	}
	return 0;
}

DEFINE_HOOK(0x418826, AircraftClass_Mission_Attack_Fire_Imcoming_2, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AbstractClass* pTarget = pTechno->Target;
	int weaponIdx = pTechno->SelectWeapon(pTarget);
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	if (pWeapon && pWeapon->WeaponType && pWeapon->WeaponType->Damage <= 0)
	{
		// skip scatter target cell
		return 0x418870;
	}
	return 0;
}

DEFINE_HOOK(0x418935, AircraftClass_Mission_Attack_Fire_Imcoming_3, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AbstractClass* pTarget = pTechno->Target;
	int weaponIdx = pTechno->SelectWeapon(pTarget);
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	if (pWeapon && pWeapon->WeaponType && pWeapon->WeaponType->Damage <= 0)
	{
		// skip scatter target cell
		return 0x41897F;
	}
	return 0;
}

DEFINE_HOOK(0x418A44, AircraftClass_Mission_Attack_Fire_Imcoming_4, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AbstractClass* pTarget = pTechno->Target;
	int weaponIdx = pTechno->SelectWeapon(pTarget);
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	if (pWeapon && pWeapon->WeaponType && pWeapon->WeaponType->Damage <= 0)
	{
		// skip scatter target cell
		return 0x418A8E;
	}
	return 0;
}

DEFINE_HOOK(0x418B40, AircraftClass_Mission_Attack_Fire_Imcoming_5, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AbstractClass* pTarget = pTechno->Target;
	int weaponIdx = pTechno->SelectWeapon(pTarget);
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	if (pWeapon && pWeapon->WeaponType && pWeapon->WeaponType->Damage <= 0)
	{
		// skip scatter target cell
		return 0x418B8A;
	}
	return 0;
}
