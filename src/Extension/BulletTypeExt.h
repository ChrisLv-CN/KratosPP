#pragma once

#include "TypeExtension.h"

#include <BulletTypeClass.h>

class BulletTypeExt : public TypeExtension<BulletTypeClass, BulletTypeExt>
{
public:
	class TypeData : public INIConfig
	{
	public:
		virtual void Read(INIBufferReader* ini) override
		{
		}

		// Ares
	};

	static constexpr DWORD Canary = 0xF00DF00D;
	static constexpr size_t ExtPointerOffset = 0x18;

	static BulletTypeExt::ExtContainer ExtMap;
};
