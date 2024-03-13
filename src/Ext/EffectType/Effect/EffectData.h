#pragma once

#include <Ext/ObjectType/FilterData.h>

enum class AffectWho : int
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

#define EFFECT_DATA(DATA_NAME) \
	virtual std::string GetEffectScriptName() override \
	{ \
		return std::string{ #DATA_NAME } + "Effect"; \
	} \
	virtual std::string GetStateScriptName() override \
	{ \
		return std::string{ #DATA_NAME } + "State"; \
	} \

class EffectData : public FilterData
{
public:
	virtual std::string GetEffectScriptName() { return ""; };
	virtual std::string GetStateScriptName() { return ""; };

	// 触发条件
	bool Powered = false; // 建筑需要使用电力

	bool CheckHealthPrecent = false; // 需要检查血量
	double ActiveWhenHealthPrecent = 1; // 血量低于这个比例再触发
	double DeactiveWhenHealthPrecent = 0; // 血量高于这个比例触发

	int TriggeredTimes = -1; // 触发次数
	AffectWho AffectWho = AffectWho::MASTER;
	bool DeactiveWhenCivilian = false;

	virtual void Read(INIBufferReader* reader) override
	{
		FilterData::Read(reader);
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		FilterData::Read(reader, title);

		Enable = reader->Get(title + "Enable", Enable);

		Powered = reader->Get(title + "Powered", Powered);
		ActiveWhenHealthPrecent = reader->GetPercent(title + "ActiveWhenHealthPrecent", ActiveWhenHealthPrecent);
		DeactiveWhenHealthPrecent = reader->GetPercent(title + "DeactiveWhenHealthPrecent", DeactiveWhenHealthPrecent);

		CheckHealthPrecent = (ActiveWhenHealthPrecent > 0.0 && ActiveWhenHealthPrecent < 1.0)
			|| (DeactiveWhenHealthPrecent > 0.0 && DeactiveWhenHealthPrecent < 1.0);

		TriggeredTimes = reader->Get(title + "TriggeredTimes", TriggeredTimes);
		AffectWho = reader->Get(title + "AffectWho", AffectWho);
		DeactiveWhenCivilian = reader->Get(title + "DeactiveWhenCivilian", DeactiveWhenCivilian);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Powered)

			.Process(this->CheckHealthPrecent)
			.Process(this->ActiveWhenHealthPrecent)
			.Process(this->DeactiveWhenHealthPrecent)

			.Process(this->TriggeredTimes)
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
