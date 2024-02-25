#pragma once

#include <TerrainTypeClass.h>

#include "TypeExtension.h"

class TerrainTypeExt : public TypeExtension<TerrainTypeClass, TerrainTypeExt>
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

	static constexpr DWORD Canary = 0xBEE78007;
	// static constexpr size_t ExtPointerOffset = 0x18;

	static TerrainTypeExt::ExtContainer ExtMap;
};
