#pragma once

#include "TypeExtension.h"

#include <WarheadTypeClass.h>

class WarheadTypeExt : public TypeExtension<WarheadTypeClass, WarheadTypeExt>
{
public:
	static constexpr DWORD Canary = 0x22222222;
	static constexpr size_t ExtPointerOffset = 0x18;

	static WarheadTypeExt::ExtContainer ExtMap;
};

static bool HasPreImpactAnim(WarheadTypeClass* pWH)
{
	return false;
}
