#pragma once

#include "TypeExtension.h"

#include <WeaponTypeClass.h>

class WeaponTypeExt : public TypeExtension<WeaponTypeClass, WeaponTypeExt>
{
public:
	static constexpr DWORD Canary = 0x22222222;
	static constexpr size_t ExtPointerOffset = 0x18;

	static WeaponTypeExt::ExtContainer ExtMap;
};
