#pragma once

#include "TypeExtension.h"

#include <SuperWeaponTypeClass.h>

class SuperWeaponTypeExt : public TypeExtension<SuperWeaponTypeClass, SuperWeaponTypeExt>
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

	static constexpr DWORD Canary = 0x11111111;
	static constexpr size_t ExtPointerOffset = 0x18;

	static SuperWeaponTypeExt::ExtContainer ExtMap;
};
