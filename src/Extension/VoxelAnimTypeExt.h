#pragma once

#include <VoxelAnimTypeClass.h>

#include "TypeExtension.h"

class VoxelAnimTypeExt : public TypeExtension<VoxelAnimTypeClass, VoxelAnimTypeExt>
{
public:
	/// @brief 储存一些通用设置或者其他平台的设置
	class TypeData : public INIConfig
	{
	public:
		virtual void Read(INIBufferReader* ini) override
		{
		}

		// Ares
	};

	static constexpr DWORD Canary = 0xAAAEEEEE;
	// static constexpr size_t ExtPointerOffset = 0x18;

	static VoxelAnimTypeExt::ExtContainer ExtMap;
};
