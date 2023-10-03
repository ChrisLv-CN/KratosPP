#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/Data/EffectData.h>
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
	std::vector<std::string> Anims{};
	bool Random = false;
	bool PlayInAir = false;

	std::string WreckType{};
	Mission WreckMission = Mission::Sleep;

	WreckOwner Owner = WreckOwner::INVOKER;

	bool Wreck = false;

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader, TITLE);

		Anims = reader->GetList(TITLE + "Types", Anims);
		Random = reader->Get(TITLE + "Random", Random);
		PlayInAir = reader->Get(TITLE + "PlayInAir", PlayInAir);

		WreckType = reader->Get(TITLE + "WreckType", WreckType);
		WreckMission = reader->Get(TITLE + "WreckMission", WreckMission);

		Owner = reader->Get(TITLE + "Owner", Owner);

		Enable = !Anims.empty() || IsNotNone(WreckType);

		Wreck = reader->Get(TITLE + "Wreck", Wreck);
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
	inline static std::string TITLE = "DestroyAnim.";
};
