#include <exception>
#include <Windows.h>

#include <AnimClass.h>
#include <TechnoClass.h>
#include <UnitClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>

#include <Ext/TechnoType/TechnoStatus.h>

#pragma region Building explosion anims

DEFINE_HOOK(0x441A26, BuildingClass_Destroy_Explosion_Remap, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EBP);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	return 0x4418EC;
}

DEFINE_HOOK(0x441B22, BuildingClass_Destroy_Exploding_Remap, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EBP);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	return 0;
}

DEFINE_HOOK(0x441D41, BuildingClass_Destroy_DestroyAnim_Remap, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EDI);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	return 0;
}
#pragma endregion

