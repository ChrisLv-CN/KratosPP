#include <exception>
#include <Windows.h>

#include <TechnoClass.h>
#include <UnitClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Ext/Helper.h>
#include <Ext/TechnoStatus.h>
#include <Extension/TechnoExt.h>

#pragma region Building explosion anims
DEFINE_HOOK(0x441A26, BuildingClass_Destroy_Explosion_Remap, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EBP);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	return 0;
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

DEFINE_HOOK(0x441D25, BuildingClass_Destroy_DestroyAnim_Remap, 0xA)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EBP);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	return 0;
}
#pragma endregion
