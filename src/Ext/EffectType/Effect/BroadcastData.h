#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>


class BroadcastEntity
{
public:
	bool Enable = false;

	std::vector<std::string> Types{};
	std::vector<double> AttachChances{};
	int Rate = 15;
	float RangeMin = 0;
	float RangeMax = -1;
	bool FullAirspace = false;

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Types = reader->GetList(title + "Types", Types);
		AttachChances = reader->GetChanceList(title + "AttachChances", AttachChances);
		Rate = reader->Get(title + "Rate", Rate);
		RangeMin = reader->Get(title + "RangeMin", RangeMin);
		RangeMax = reader->Get(title + "RangeMax", RangeMax);
		FullAirspace = reader->Get(title + "FullAirspace", FullAirspace);

		// 0 时关闭，-1全地图
		Enable = !Types.empty() && RangeMax != 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Types)
			.Process(this->AttachChances)
			.Process(this->Rate)
			.Process(this->RangeMin)
			.Process(this->RangeMax)
			.Process(this->FullAirspace)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<BroadcastEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class BroadcastData : public EffectData
{
public:
	EFFECT_DATA(Broadcast);

	BroadcastEntity Data{};
	BroadcastEntity EliteData{};

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader, TITLE);

		BroadcastEntity data;
		data.Read(reader, TITLE);
		if (data.Enable)
		{
			Data = data;
			EliteData = Data;
		}

		BroadcastEntity eliteData;
		eliteData.Read(reader, TITLE + "Elite");
		if (eliteData.Enable)
		{
			EliteData = eliteData;
		}

		Enable = Data.Enable || EliteData.Enable;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)
			.Process(this->EliteData)
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
		return const_cast<BroadcastData*>(this)->Serialize(stream);
	}
#pragma endregion

protected:
	inline static std::string TITLE = "Broadcast.";
};
