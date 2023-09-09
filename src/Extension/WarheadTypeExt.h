#pragma once

#include "TypeExtension.h"

#include <WarheadTypeClass.h>

class WarheadTypeExt : public TypeExtension<WarheadTypeClass, WarheadTypeExt>
{
public:
	class TypeData : public INIConfig
	{
	public:
		virtual void Read(INIBufferReader* ini) override
		{

			IsToy = ini->Get("IsToy", IsToy);
			Lueluelue = ini->Get("Lueluelue", Lueluelue);
		}

		// Ares

		// Kratos
		bool IsToy = false;
		bool Lueluelue = false;
	};

	static constexpr DWORD Canary = 0x22222222;
	static constexpr size_t ExtPointerOffset = 0x18;

	static WarheadTypeExt::ExtContainer ExtMap;
};

static bool HasPreImpactAnim(WarheadTypeClass* pWH)
{
	return false;
}
