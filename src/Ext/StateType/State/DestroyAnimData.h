#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>

enum class WreckOwner
{
	INVOKER = 0, KILLER = 1, NEUTRAL = 2
};

template <>
inline bool Parser<WreckOwner>::TryParse(const char* pValue, WreckOwner* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'K':
		if (outValue)
		{
			*outValue = WreckOwner::KILLER;
		}
		return true;
	case 'N':
		if (outValue)
		{
			*outValue = WreckOwner::NEUTRAL;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = WreckOwner::INVOKER;
		}
		return true;
	}
}

class DestroyAnimData : public EffectData
{
public:
	EFFECT_DATA(DestroyAnim);

	std::vector<std::string> Anims{};
	bool Random = false;
	bool PlayInAir = false;

	std::string WreckType{};
	Mission WreckMission = Mission::Sleep;

	WreckOwner Owner = WreckOwner::INVOKER;

	// 单位标签，我是个残骸
	bool Wreck = false;

	virtual void Read(INIBufferReader* reader)
	{
		EffectData::Read(reader);
		Read(reader, "DestroyAnim.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Anims = reader->GetList(title + "Types", Anims);
		ClearIfGetNone(Anims);
		Random = reader->Get(title + "Random", Random);
		PlayInAir = reader->Get(title + "PlayInAir", PlayInAir);

		WreckType = reader->Get(title + "WreckType", WreckType);
		WreckMission = reader->Get(title + "WreckMission", WreckMission);

		Owner = reader->Get(title + "Owner", Owner);

		Enable = !Anims.empty() || IsNotNone(WreckType);

		// 单位标签，我是个残骸
		Wreck = reader->Get("Wreck", Wreck);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Anims)
			.Process(this->Random)
			.Process(this->PlayInAir)

			.Process(this->WreckType)
			.Process(this->WreckMission)

			.Process(this->Owner)
			.Process(this->Wreck)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		EffectData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectData::Save(stream);
		return const_cast<DestroyAnimData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
