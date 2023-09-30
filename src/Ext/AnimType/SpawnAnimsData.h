#pragma once

#include <Common/INI/INIConfig.h>
#include <Ext/ExpandAnims/ExpandAnimsData.h>

class SpawnAnimsData : public ExpandAnimsData
{
public:
	bool TriggerOnDone = true;
	bool TriggerOnNext = false;
	bool TriggerOnLoop = false;
	bool TriggerOnStart = false;
	int Count = 1;

	int InitDelay = 0;
	bool UseRandomInitDelay = false;
	Point2D RandomInitDelay = Point2D::Empty;

	int Delay = 0;
	bool UseRandomDelay = false;
	Point2D RandomDelay = Point2D::Empty;

	virtual void Read(INIBufferReader* reader) override
	{
		ExpandAnimsData::Read(reader, TITLE);

		TriggerOnDone = reader->Get(TITLE + "TriggerOnDone", TriggerOnDone);
		TriggerOnNext = reader->Get(TITLE + "TriggerOnNext", TriggerOnNext);
		TriggerOnLoop = reader->Get(TITLE + "TriggerOnLoop", TriggerOnLoop);

		TriggerOnStart = reader->Get(TITLE + "TriggerOnStart", TriggerOnStart);
		Count = reader->Get(TITLE + "Count", Count);

		InitDelay = reader->Get(TITLE + "InitDelay", InitDelay);
		RandomInitDelay = reader->Get(TITLE + "RandomInitDelay", RandomInitDelay);
		UseRandomInitDelay = !RandomInitDelay.IsEmpty();

		Delay = reader->Get(TITLE + "Delay", Delay);
		RandomDelay = reader->Get(TITLE + "RandomDelay", RandomDelay);
		UseRandomDelay = !RandomDelay.IsEmpty();

		Enable = Enable && (TriggerOnDone || TriggerOnNext || TriggerOnLoop || TriggerOnStart);
	}

	int GetInitDelay()
	{
		if (UseRandomInitDelay)
		{
			return GetRandomValue(RandomInitDelay, 0);
		}
		return InitDelay;
	}

	int GetDelay()
	{
		if (UseRandomDelay)
		{
			return GetRandomValue(RandomDelay, 0);
		}
		return Delay;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->TriggerOnDone)
			.Process(this->TriggerOnNext)
			.Process(this->TriggerOnLoop)
			.Process(this->TriggerOnStart)
			.Process(this->Count)

			.Process(this->InitDelay)
			.Process(this->UseRandomInitDelay)
			.Process(this->RandomInitDelay)

			.Process(this->Delay)
			.Process(this->UseRandomDelay)
			.Process(this->RandomDelay)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		ExpandAnimsData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		ExpandAnimsData::Save(stream);
		return const_cast<SpawnAnimsData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "Spawns.";

};

