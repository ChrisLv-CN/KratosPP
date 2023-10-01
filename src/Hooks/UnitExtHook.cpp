#include <exception>
#include <Windows.h>

#include <TechnoClass.h>
#include <UnitClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Ext/Helper.h>
#include <Ext/TechnoStatus.h>
#include <Extension/TechnoExt.h>

#pragma region Unit Deploy
DEFINE_HOOK(0x6FF929, TechnoClass_Fire_FireOnce, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	if (pTechno->CurrentMission == Mission::Unload)
	{
		pTechno->QueueMission(Mission::Stop, true);
	}
	return 0;
}

DEFINE_HOOK(0x739C74, UnitClass_Deployed, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		status->OnUpdate_DeployToTransform();
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
