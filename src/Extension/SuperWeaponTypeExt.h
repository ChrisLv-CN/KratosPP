#pragma once

#include "TypeExtension.h"

#include <SuperWeaponTypeClass.h>

class SuperWeaponTypeExt : public TypeExtension<SuperWeaponTypeClass, SuperWeaponTypeExt>
{
public:
	static constexpr DWORD Canary = 0x11111111;
	static constexpr size_t ExtPointerOffset = 0x18;

	static SuperWeaponTypeExt::ExtContainer ExtMap;
};
