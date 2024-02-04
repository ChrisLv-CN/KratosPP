#pragma once

#include "Ext/ObjectType/FilterData.h"

enum class AffectWho
{
	MASTER = 0, STAND = 1, ALL = 2,
};

template <>
inline bool Parser<AffectWho>::TryParse(const char* pValue, AffectWho* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'M':
		if (outValue)
		{
			*outValue = AffectWho::MASTER;
		}
		return true;
	case 'S':
		if (outValue)
		{
			*outValue = AffectWho::STAND;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = AffectWho::ALL;
		}
		return true;
	}
}

class EffectData : public FilterData
{
public:

	int TriggeredTimes = -1; // 触发次数
	bool Powered = false; // 建筑需要使用电力
	AffectWho AffectWho = AffectWho::MASTER;
	bool DeactiveWhenCivilian = false;

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		FilterData::Read(reader, title);

		Enable = reader->Get(title + "Enable", Enable);
		TriggeredTimes = reader->Get(title + "TriggeredTimes", TriggeredTimes);
		Powered = reader->Get(title + "Powered", Powered);
		AffectWho = reader->Get(title + "AffectWho", AffectWho);
		DeactiveWhenCivilian = reader->Get(title + "DeactiveWhenCivilian", DeactiveWhenCivilian);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->TriggeredTimes)
			.Process(this->Powered)
			.Process(this->AffectWho)
			.Process(this->DeactiveWhenCivilian)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		FilterData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		FilterData::Save(stream);
		return const_cast<EffectData*>(this)->Serialize(stream);
	}
#pragma endregion
};
