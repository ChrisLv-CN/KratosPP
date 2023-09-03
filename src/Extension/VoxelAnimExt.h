#pragma once

#include "GOExtension.h"

#include <VoxelAnimClass.h>

class VoxelAnimExt : public GOExtension<VoxelAnimClass, VoxelAnimExt>
{
public:
	static constexpr DWORD Canary = 0xAAAAAACC;
	static constexpr size_t ExtPointerOffset = 0x18;

	static void AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext);

	static VoxelAnimExt::ExtContainer ExtMap;
};
