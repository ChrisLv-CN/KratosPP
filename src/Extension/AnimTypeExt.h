#pragma once

#include <AnimTypeClass.h>

#include "TypeExtension.h"

class AnimTypeExt : public TypeExtension<AnimTypeClass, AnimTypeExt>
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

	static constexpr DWORD Canary = 0xEEEEEEEE;
	// static constexpr size_t ExtPointerOffset = 0x18;

	static AnimTypeExt::ExtContainer ExtMap;
};
