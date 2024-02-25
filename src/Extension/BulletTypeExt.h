#pragma once

#include <BulletTypeClass.h>

#include "TypeExtension.h"

class BulletTypeExt : public TypeExtension<BulletTypeClass, BulletTypeExt>
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

	static constexpr DWORD Canary = 0xF00DF00D;
	// static constexpr size_t ExtPointerOffset = 0x18;

	static BulletTypeExt::ExtContainer ExtMap;
};
