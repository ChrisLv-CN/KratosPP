#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>



class PumpData : public EffectData
{
public:
	EFFECT_DATA(Pump);

	int Range = 0; // 飞多远，距离
	int Gravity = 0; // 重力
	bool PowerBySelf = false; // 由自身提供动力，来源只控制方向

	bool Lobber = false; // 高抛
	bool Inaccurate = false; // 不精准
	float ScatterMin = 0; // 散布范围
	float ScatterMax = 0; // 散布范围

	Sequence InfSequence = Sequence::Ready;

	PumpData() : EffectData()
	{
		AffectBuilding = false;
		AffectBullet = false;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Pump.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Range = reader->Get(title + "Range", Range);

		Gravity = reader->Get(title + "Gravity", Gravity);
		PowerBySelf = reader->Get(title + "PowerBySelf", PowerBySelf);

		Lobber = reader->Get(title + "Lobber", Lobber);
		Inaccurate = reader->Get(title + "Inaccurate", Inaccurate);
		ScatterMin = reader->Get(title + "ScatterMin", ScatterMin);
		ScatterMax = reader->Get(title + "ScatterMax", ScatterMax);

		InfSequence = reader->Get(title + "InfSequence", InfSequence);

		Enable = Range > 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Range)
			.Process(this->Gravity)
			.Process(this->PowerBySelf)

			.Process(this->Lobber)
			.Process(this->Inaccurate)
			.Process(this->ScatterMin)
			.Process(this->ScatterMax)
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
		return const_cast<PumpData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
