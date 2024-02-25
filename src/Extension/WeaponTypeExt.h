#pragma once

#include <WeaponTypeClass.h>

#include "Ext/ObjectType/AttachFireData.h"

#include "TypeExtension.h"

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

		int NoMoneyNoTalk = 0;
		bool DontNeedMoney = false;

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

			NoMoneyNoTalk = ini->Get("NoMoneyNoTalk", NoMoneyNoTalk);
			DontNeedMoney = ini->Get("DontNeedMoney", DontNeedMoney);
		}

#pragma region save/load
		template <typename T>
		bool Serialize(T& stream)
		{
			return stream
				.Process(this->Ammo)

				.Process(this->LaserThickness)
				.Process(this->LaserFade)
				.Process(this->IsSupported)

				.Process(this->RockerPitch)
				.Process(this->SelfLaunch)
				.Process(this->PumpAction)
				.Process(this->HumanCannon)

				.Process(this->NoMoneyNoTalk)
				.Process(this->DontNeedMoney)
				.Success();
		};

		virtual bool Load(ExStreamReader& stream, bool registerForChange) override
		{
			AttachFireData::Load(stream, registerForChange);
			return this->Serialize(stream);
		}
		virtual bool Save(ExStreamWriter& stream) const override
		{
			AttachFireData::Save(stream);
			return const_cast<TypeData*>(this)->Serialize(stream);
		}
#pragma endregion
	};

	static constexpr DWORD Canary = 0x22222222;
	// static constexpr size_t ExtPointerOffset = 0x18;

	static WeaponTypeExt::ExtContainer ExtMap;
};
