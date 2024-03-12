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

// GiftBox will release a building onto a cell of existing buildings.
// if that gift is not a virtual unit like Stand. I will add it to the occupy objects,
// When a building want to clear occupy spot, check if there has another building at the same cell,
// Skip the change the OccFlag of this cell.
DEFINE_HOOK(0x453E02, BuildingClass_Clear_Occupy_Spot_Skip, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(CellClass*, pCell, EAX);
	ObjectClass* pObject = pCell->FirstObject;
	do
	{
		if (pObject)
		{
			switch (pObject->WhatAmI())
			{
			case AbstractType::Building:
			{
				if (pObject != pTechno)
				{
					// skip change the OccFlag of this cell
					return 0x453E12;
				}
				break;
			}
			}
		}
	} while (pObject && (pObject = pObject->NextObject) != nullptr);
	return 0;
}

