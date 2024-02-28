#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>


class AttackBeaconEntity
{
public:
	bool Enable = false;
	std::vector<std::string> Types{};
	std::vector<int> Nums{};
	int Rate = 0;
	float RangeMin = 0;
	float RangeMax = -1;
	bool Force = false;
	int Count = 1;
	bool TargetToCell = false;

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Enable = reader->Get(title + "Enable", Enable);

		Types = reader->GetList(title + "Types", Types);
		ClearIfGetNone(Types);

		Nums = reader->GetList(title + "Nums", Nums);

		Rate = reader->Get(title + "Rate", Rate);

		RangeMin = reader->Get(title + "RangeMin", RangeMin);
		RangeMax = reader->Get(title + "RangeMax", RangeMax);

		Force = reader->Get(title + "Force", Force);
		Count = reader->Get(title + "Count", Count);
		TargetToCell = reader->Get(title + "TargetToCell", TargetToCell);
	}

	bool Contains(std::string typeId)
	{
		bool mark = Types.empty();
		if (!mark)
		{
			auto it = std::find(Types.begin(), Types.end(), typeId);
			mark = it != Types.end();
		}
		return mark;
	}

	std::map<std::string, int> GetNumbers()
	{
		std::map<std::string, int> numbers;
		int index = 0;
		for (std::string typeId : Types)
		{
			int num = -1;
			int size = Nums.size();
			if (index < size)
			{
				num = Nums[index];
			}
			numbers[typeId] = num;
			index++;
		}
		return numbers;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Types)
			.Process(this->Nums)
			.Process(this->Rate)
			.Process(this->RangeMin)
			.Process(this->RangeMax)
			.Process(this->Force)
			.Process(this->Count)
			.Process(this->TargetToCell)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<AttackBeaconEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class AttackBeaconData : public EffectData
{
public:
	EFFECT_DATA(AttackBeacon);

	AttackBeaconEntity Data{};
	AttackBeaconEntity EliteData{};

	AttackBeaconData() : EffectData()
	{
		AffectsOwner = true;
		AffectsAllies = false;
		AffectsEnemies = false;
		AffectsCivilian = false;
		DeactiveWhenCivilian = true;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "AttackBeacon.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		AttackBeaconEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Data = data;
			EliteData = Data;
		}

		AttackBeaconEntity eliteData;
		eliteData.Read(reader, title + "Elite");
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
		return const_cast<AttackBeaconData*>(this)->Serialize(stream);
	}
#pragma endregion
};

