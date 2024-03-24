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

enum class StackActionMode : int
{
	OR = 0, AND = 1,
};

template <>
inline bool Parser<StackActionMode>::TryParse(const char* pValue, StackActionMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case '1':
	case 'T':
	case 'Y':
	case 'A':
		if (outValue)
		{
			*outValue = StackActionMode::AND;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = StackActionMode::OR;
		}
		return true;
	}
}

class StackData : public EffectData
{
public:
	EFFECT_DATA(Stack);

	std::vector<std::string> Watch{};

	std::vector<int> Level{};
	std::vector<Condition> Condition{};

	StackActionMode ActionMode = StackActionMode::OR;

	bool Attach = false;
	std::vector<std::string> AttachEffects{};
	std::vector<double> AttachChances{};

	bool Remove = false;
	std::vector<std::string> RemoveEffects{};
	std::vector<std::string> RemoveEffectsWithMarks{};
	std::vector<int> RemoveEffectsLevel{};

	std::vector<int> RemoveLevel{};
	bool RemoveAll = true;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Stack.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Watch = reader->GetList(title + "Watch", Watch);

		Level = reader->GetList(title + "Level", Level);
		Condition = reader->GetList(title + "Condition", Condition);

		ActionMode = reader->Get(title + "ActionMode", ActionMode);

		AttachEffects = reader->GetList(title + "AttachEffects", AttachEffects);
		ClearIfGetNone(AttachEffects);
		AttachChances = reader->GetChanceList(title + "AttachChances", AttachChances);
		Attach = !AttachEffects.empty();

		RemoveEffects = reader->GetList(title + "RemoveEffects", RemoveEffects);
		ClearIfGetNone(RemoveEffects);
		RemoveEffectsWithMarks = reader->GetList(title + "RemoveEffectsWithMarks", RemoveEffectsWithMarks);
		ClearIfGetNone(RemoveEffectsWithMarks);
		RemoveLevel = reader->GetList(title + "RemoveEffectsLevel", RemoveLevel);
		Remove = !RemoveEffects.empty() || !RemoveEffectsWithMarks.empty();

		RemoveLevel = reader->GetList(title + "RemoveLevel", RemoveLevel);
		RemoveAll = reader->Get(title + "RemoveAll", RemoveAll);

		Enable = !Watch.empty() && (Attach || Remove);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Watch)
			.Process(this->Level)
			.Process(this->Condition)
			.Process(this->ActionMode)
			.Process(this->Attach)
			.Process(this->AttachEffects)
			.Process(this->AttachChances)
			.Process(this->Remove)
			.Process(this->RemoveEffects)
			.Process(this->RemoveLevel)
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
