#pragma once

#include "TypeExtension.h"

#include <VoxelAnimTypeClass.h>

class VoxelAnimTypeExt : public TypeExtension<VoxelAnimTypeClass, VoxelAnimTypeExt>
{
public:
	static constexpr DWORD Canary = 0xAAAEEEEE;
	static constexpr size_t ExtPointerOffset = 0x18;

	static VoxelAnimTypeExt::ExtContainer ExtMap;
};
