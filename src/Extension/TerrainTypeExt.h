#pragma once

#include "TypeExtension.h"

#include <TerrainTypeClass.h>

class TerrainTypeExt : public TypeExtension<TerrainTypeClass, TerrainTypeExt>
{
public:
	static constexpr DWORD Canary = 0xBEE78007;
	static constexpr size_t ExtPointerOffset = 0x18;

	static TerrainTypeExt::ExtContainer ExtMap;
};
