#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>

enum class Condition : int
{
	EQ = 0,
	NE = 1,
	GT = 2,
	LT = 3,
	GE = 4,
	LE = 5,
};

static std::map<std::string, Condition> ConditionTypeStrings
{
	{ "eq", Condition::EQ },
	{ "ne", Condition::NE },
	{ "gt", Condition::GT },
	{ "lt", Condition::LT },
	{ "ge", Condition::GE },
	{ "le", Condition::LE }
};

template <>
inline bool Parser<Condition>::TryParse(const char* pValue, Condition* outValue)
{
	std::string key = lowercase(std::string(pValue));
	auto it = ConditionTypeStrings.find(key);
	if (it != ConditionTypeStrings.end())
	{
		*outValue = it->second;
		return true;
	}
	return false;
}

class StackData : public EffectData
{
public:
	EFFECT_DATA(Stack);

	std::string Watch{ "" };

	int Level = 0;
	Condition Condition = Condition::EQ;

	bool Attach = false;
	std::vector<std::string> AttachEffects{};
	std::vector<double> AttachChances{};

	bool Remove = false;
	std::vector<std::string> RemoveEffects{};

	bool RemoveAll = true;

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader);
		Read(reader, "Stack.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);
		Watch = reader->Get(title + "Watch", Watch);

		Level = reader->Get(title + "Level", Level);
		Condition = reader->Get(title + "Condition", Condition);

		AttachEffects = reader->GetList(title + "AttachEffects", AttachEffects);
		AttachChances = reader->GetChanceList(title + "AttachChances", AttachChances);
		Attach = !AttachEffects.empty();

		RemoveEffects = reader->GetList(title + "RemoveEffects", RemoveEffects);
		Remove = !RemoveEffects.empty();

		RemoveAll = reader->Get(title + "RemoveAll", RemoveAll);

		Enable = IsNotNone(Watch) && (Attach || Remove);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Watch)
			.Process(this->Level)
			.Process(this->Condition)
			.Process(this->Attach)
			.Process(this->AttachEffects)
			.Process(this->AttachChances)
			.Process(this->Remove)
			.Process(this->RemoveEffects)
			.Process(this->RemoveAll)
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
		return const_cast<StackData*>(this)->Serialize(stream);
	}
#pragma endregion
};
