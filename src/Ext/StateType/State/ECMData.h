#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


enum class ECMAround : int
{
	Source = 0, Target = 1, Self = 2, Shooter = 3
};

static std::map<std::string, ECMAround> ECMAroundStrings
{
	{ "Source", ECMAround::Source },
	{ "Target", ECMAround::Target },
	{ "Self", ECMAround::Self },
	{ "Shooter", ECMAround::Shooter }
};

template <>
inline bool Parser<ECMAround>::TryParse(const char* pValue, ECMAround* outValue)
{
	std::string key = pValue;
	auto it = ECMAroundStrings.find(key);
	if (it != ECMAroundStrings.end())
	{
		*outValue = it->second;
		return true;
	}
	return false;
}

class ECMData : public EffectData
{
public:
	EFFECT_DATA(ECM);

	double Chance = 0; // 发生的概率
	int Rate = 15;

	int LockDuration = 0; // 锁定时间
	bool Feedback = false; // 反噬

	double ToTechnoChance = 0; // 新目标是单位的概率
	bool ForceRetarget = false; // 一定重置目标
	bool NoOwner = false; // 清除抛射体的发射者

	ECMAround Around = ECMAround::Source; // 新目标搜索范围圆心

	double RangeMin = 0;
	double RangeMax = 1;
	bool FullAirspace = false;

	ECMData() : EffectData()
	{
		TriggeredTimes = 1;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "ECM.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Chance = reader->GetChance(title + "Chance", Chance);
		Rate = reader->Get(title + "Rate", Rate);

		LockDuration = reader->Get(title + "LockDuration", LockDuration);
		Feedback = reader->Get(title + "Feedback", Feedback);

		ToTechnoChance = reader->GetChance(title + "ToTechnoChance", ToTechnoChance);
		ForceRetarget = reader->Get(title + "ForceRetarget", ForceRetarget);
		NoOwner = reader->Get(title + "NoOwner", NoOwner);

		Around = reader->Get(title + "Around", Around);
		RangeMax = reader->Get(title + "RangeMax", RangeMax);
		if (RangeMax < 0)
		{
			RangeMax = 0;
		}
		RangeMin = reader->Get(title + "RangeMin", RangeMin);
		if (RangeMin < 0)
		{
			RangeMin = 0;
		}
		if (RangeMin > RangeMax)
		{
			RangeMin = RangeMax;
		}
		FullAirspace = reader->Get(title + "FullAirspace", FullAirspace);

		Enable = Chance > 0 && (Feedback || RangeMax > 0);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Chance)
			.Process(this->Rate)
			.Process(this->LockDuration)
			.Process(this->Feedback)
			.Process(this->ToTechnoChance)
			.Process(this->ForceRetarget)
			.Process(this->NoOwner)
			.Process(this->Around)
			.Process(this->RangeMin)
			.Process(this->RangeMax)
			.Process(this->FullAirspace)
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
		return const_cast<ECMData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
