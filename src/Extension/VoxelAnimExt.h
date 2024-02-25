#pragma once

#include <FootClass.h>
#include <VoxelAnimClass.h>

#include "GOExtension.h"


class VoxelAnimExt : public GOExtension<VoxelAnimClass, VoxelAnimExt>
{
public:
	static constexpr DWORD Canary = 0xAAAAAACC;
	// static constexpr size_t ExtPointerOffset = 0x18;

	/// @brief 创建Component实例，并加入到GameObject中.
	/// 创建时需要使用new，不能使用GameCreate.
	/// @param globalScripts 待附加的脚本列表
	/// @param ext ExtData
	static void AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext);

	static VoxelAnimExt::ExtContainer ExtMap;
};
