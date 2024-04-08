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

		// Phobos
		ColorStruct BoltColor1 = Colors::Empty;
		ColorStruct BoltColor2 = Colors::Empty;
		ColorStruct BoltColor3 = Colors::Empty;
		bool BoltDisable1 = false;
		bool BoltDisable2 = false;
		bool BoltDisable3 = false;
		bool BoltDisableParticle = false;

		// Kratos
		float RockerPitch = 0;
		bool SelfLaunch = false;
		bool PumpAction = false;
		Sequence PumpInfSequence = Sequence::Crawl;
		int HumanCannon = -1;

		int NoMoneyNoTalk = 0;
		bool DontNeedMoney = false;

		virtual void Read(INIBufferReader* reader) override
		{
			AttachFireData::Read(reader);

			Ammo = reader->Get("Ammo", Ammo);

			LaserThickness = reader->Get("LaserThickness", LaserThickness);
			LaserFade = reader->Get("LaserFade", LaserFade);
			IsSupported = reader->Get("IsSupported", IsSupported);

			BoltColor1 = reader->Get("Bolt.Color1", BoltColor1);
			BoltColor2 = reader->Get("Bolt.Color2", BoltColor2);
			BoltColor3 = reader->Get("Bolt.Color3", BoltColor3);

			BoltDisable1 = reader->Get("Bolt.Disable1", BoltDisable1);
			BoltDisable2 = reader->Get("Bolt.Disable2", BoltDisable2);
			BoltDisable3 = reader->Get("Bolt.Disable3", BoltDisable3);
			BoltDisableParticle = reader->Get("Bolt.DisableParticle", BoltDisableParticle);

			RockerPitch = reader->Get("RockerPitch", RockerPitch);
			SelfLaunch = reader->Get("SelfLaunch", SelfLaunch);
			PumpAction = reader->Get("PumpAction", PumpAction);
			PumpInfSequence = reader->Get("PumpAction.InfSequence", PumpInfSequence);
			HumanCannon = reader->Get("HumanCannon", HumanCannon);

			NoMoneyNoTalk = reader->Get("NoMoneyNoTalk", NoMoneyNoTalk);
			DontNeedMoney = reader->Get("DontNeedMoney", DontNeedMoney);
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

				.Process(this->BoltColor1)
				.Process(this->BoltColor2)
				.Process(this->BoltColor3)
				.Process(this->BoltDisable1)
				.Process(this->BoltDisable2)
				.Process(this->BoltDisable3)
				.Process(this->BoltDisableParticle)

				.Process(this->RockerPitch)
				.Process(this->SelfLaunch)
				.Process(this->PumpAction)
				.Process(this->PumpInfSequence)
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
