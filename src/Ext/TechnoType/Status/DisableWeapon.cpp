#include "../TechnoStatus.h"

#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/Scripts.h>

bool TechnoStatus::CanFire_DisableWeapon(AbstractClass* pTarget, WeaponTypeClass* pWeapon)
{
	if (DisableWeapon->IsAlive())
	{
		DisableWeaponData data = DisableWeapon->Data;
		if (!data.OnLandTypes.empty())
		{
			CoordStruct loco = pTechno->GetCoords();
			if (CellClass* pCell = MapClass::Instance->TryGetCellAt(loco))
			{
				LandType landType = pCell->LandType;
				if (landType == LandType::Water && pCell->Flags & CellFlags::Bridge)
				{
					landType = LandType::Road;
				}
				std::vector<LandType> types = data.OnLandTypes;
				auto it = std::find(types.begin(), types.end(), landType);
				return it != types.end();
			}
		}
		return true;
	}
	return false;
}

