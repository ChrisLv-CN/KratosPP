#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class NoMoneyNoTalkData : public EffectData
{
public:
	EFFECT_DATA(NoMoneyNoTalk);

	int Money = 0;

	std::vector<LandType> OnLandTypes{};

	NoMoneyNoTalkData() : EffectData()
	{
		AffectWho = AffectWho::ALL;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader);
		Read(reader, "NoMoneyNoTalk.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Money = reader->Get("NoMoneyNoTalk", Money);

		Enable = Money != 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(Money)
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
		return const_cast<NoMoneyNoTalkData*>(this)->Serialize(stream);
	}
#pragma endregion
};
