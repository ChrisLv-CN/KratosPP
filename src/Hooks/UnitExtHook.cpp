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
