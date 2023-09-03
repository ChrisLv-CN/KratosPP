#pragma once

#include "TypeExtension.h"

#include <AnimTypeClass.h>

class AnimTypeExt : public TypeExtension<AnimTypeClass, AnimTypeExt>
{
public:
	static constexpr DWORD Canary = 0xEEEEEEEE;
	static constexpr size_t ExtPointerOffset = 0x18;

	static AnimTypeExt::ExtContainer ExtMap;
};
