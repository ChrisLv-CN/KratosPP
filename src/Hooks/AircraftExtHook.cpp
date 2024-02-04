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
		Matrix3D matrix = static_cast<FootClass*>(pTechno)->Locomotor->Draw_Matrix(nullptr);
		double scale = Math::cos(abs(matrix.GetYRotation()));
		pMatrix->ScaleX(static_cast<float>(scale));
	}
	return 0;
}
#pragma endregion


#pragma region Aircraft Attitude
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
#pragma endregion
