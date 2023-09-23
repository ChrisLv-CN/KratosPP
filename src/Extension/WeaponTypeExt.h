#pragma once

#include "TypeExtension.h"
#include "Ext/WeaponType/AttachFireData.h"

#include <WeaponTypeClass.h>

class WeaponTypeExt : public TypeExtension<WeaponTypeClass, WeaponTypeExt>
{
public:
	/// @brief 储存一些通用设置或者其他平台的设置
	class TypeData : public AttachFireData
	{
	public:
		// Ares
		int Ammo = 1;

		int LaserThickness = 0;
		bool LaserFade = false;
		bool IsSupported = false;

		// Kratos
		float RockerPitch = 0;
		bool SelfLaunch = false;
		bool PumpAction = false;
		int HumanCannon = -1;

		virtual void Read(INIBufferReader* ini) override
		{
			AttachFireData::Read(ini);

			Ammo = ini->Get("Ammo", Ammo);

			LaserThickness = ini->Get("LaserThickness", LaserThickness);
			LaserFade = ini->Get("LaserFade", LaserFade);
			IsSupported = ini->Get("IsSupported", IsSupported);

			RockerPitch = ini->Get("RockerPitch", RockerPitch);
			SelfLaunch = ini->Get("SelfLaunch", SelfLaunch);
			PumpAction = ini->Get("PumpAction", PumpAction);
			HumanCannon = ini->Get("HumanCannon", HumanCannon);
		}

	};

	static constexpr DWORD Canary = 0x22222222;
	static constexpr size_t ExtPointerOffset = 0x18;

	static WeaponTypeExt::ExtContainer ExtMap;
};
