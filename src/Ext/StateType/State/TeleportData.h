#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


enum class TeleportMode : int
{
	NONE = 0, MOVE = 1, WARHEAD = 2, BOTH = 3
};

template <>
inline bool Parser<TeleportMode>::TryParse(const char* pValue, TeleportMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'M':
		if (outValue)
		{
			*outValue = TeleportMode::MOVE;
		}
		return true;
	case 'W':
		if (outValue)
		{
			*outValue = TeleportMode::WARHEAD;
		}
		return true;
	case 'B':
		if (outValue)
		{
			*outValue = TeleportMode::BOTH;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = TeleportMode::NONE;
		}
		return true;
	}
}

class TeleportData : public EffectData
{
public:
	EFFECT_DATA(Teleport);

	TeleportMode Mode = TeleportMode::NONE;

	CoordStruct Offset = CoordStruct::Empty;
	bool IsOnTarget = false;

	CoordStruct MoveTo = CoordStruct::Empty;
	bool IsOnTurret = false;

	bool Super = false;

	int Delay = 0;
	bool ClearTarget = true;
	bool MoveForward = true;

	double RangeMin = 0;
	double RangeMax = -1;
	int Distance = -1;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Teleport.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Mode = reader->Get(title + "Mode", Mode);

		Offset = reader->Get(title + "Offset", Offset);
		IsOnTarget = reader->Get(title + "IsOnTarget", IsOnTarget);

		MoveTo = reader->Get(title + "MoveTo", MoveTo);
		IsOnTurret = reader->Get(title + "IsOnTurret", IsOnTurret);

		Super = reader->Get(title + "Super", Super);

		Delay = reader->Get(title + "Delay", Delay);
		ClearTarget = reader->Get(title + "ClearTarget", ClearTarget);
		MoveForward = reader->Get(title + "MoveForward", MoveForward);

		RangeMin = reader->Get(title + "RangeMin", RangeMin);
		RangeMax = reader->Get(title + "RangeMax", RangeMax);
		Distance = reader->Get(title + "Distance", Distance);

		Enable = Mode != TeleportMode::NONE;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Mode)
			.Process(this->Offset)
			.Process(this->IsOnTarget)
			.Process(this->MoveTo)
			.Process(this->IsOnTurret)
			.Process(this->Super)
			.Process(this->Delay)
			.Process(this->ClearTarget)
			.Process(this->MoveForward)
			.Process(this->RangeMin)
			.Process(this->RangeMax)
			.Process(this->Distance)
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
		return const_cast<TeleportData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
