#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <AircraftClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>
#include <Ext/Helper.h>
#include <Ext/TechnoStatus.h>
#include <Extension/TechnoExt.h>
#include <Extension/WarheadTypeExt.h>
#include <Common/Components/Component.h>
#include <Common/Components/ScriptComponent.h>

DEFINE_HOOK(0x7067F1, TechnoClass_DrawVxl_DisableCache, 0x6)
{
	GET(unsigned int, esi, ESI);
	GET(unsigned int, eax, EAX);

	if (esi != eax)
	{
		GET(TechnoClass*, pTechno, ECX);
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status) && status->DisableVoxelCache)
		{
			// 强制禁用缓存
			return 0x706875;
		}
		return 0x7067F7;
	}
	return 0x706879;
}

#pragma region DrawShadow
DEFINE_HOOK_AGAIN(0x73C4FF, TechnoClass_DrawShadow, 0x5) // InAir
DEFINE_HOOK_AGAIN(0x73C595, TechnoClass_DrawShadow, 0x5) // OnGround
DEFINE_HOOK(0x414876, TechnoClass_DrawShadow, 0x7) // Aircraft
{
	GET(TechnoClass*, pTechno, EBP);
	GET(Matrix3D*, pMatrix, EAX);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt, TechnoStatus>(pTechno, status)
		&& (status->IsAircraft() || pTechno->GetTechnoType()->ConsideredAircraft))
	{
		pMatrix->Scale(status->VoxelShadowScaleInAir);
	}
	// 调整倾斜时影子的纵向比例
	FootClass* pFoot = nullptr;
	if (CastToFoot(pTechno, pFoot))
	{
		Matrix3D matrix = pFoot->Locomotor->Draw_Matrix(nullptr);
		double scale = Math::cos(abs(matrix.GetYRotation()));
		pMatrix->ScaleX(static_cast<float>(scale));
	}
	return 0;
}
#pragma endregion
