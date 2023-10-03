#pragma once

#include <string>
#include <map>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>
#include <Ext/Helper.h>
#include <Ext/State/FireSuperData.h>

struct FireSuper
{
public:
	int HouseIndex = -1;
	CellStruct TargetPos{};
	FireSuperEntity Data{};

	FireSuper()
	{ }

	FireSuper(HouseClass* pHouse, CellStruct targetPos, FireSuperEntity data)
	{
		this->HouseIndex = pHouse->ArrayIndex;
		this->TargetPos = targetPos;
		this->Data = data;

		this->count = 0;
		this->initDelay = GetRandomValue(data.RandomInitDelay, data.InitDelay);
		this->initDelayTimer.Start(initDelay);
		this->delay = GetRandomValue(data.RandomDelay, data.Delay);
		this->delayTimer.Start(0);
	}

	HouseClass* GetHouse()
	{
		HouseClass* pHouse = nullptr;
		if (HouseIndex > -1)
		{
			pHouse = HouseClass::Array->GetItem(HouseIndex);
		}
		return pHouse;
	}

	bool CanLaunch()
	{
		return initDelayTimer.Expired() && delayTimer.Expired();
	}

	bool IsDone()
	{
		return Data.LaunchCount > 0 && count >= Data.LaunchCount;
	}

	bool Cooldown()
	{
		count++;
		delayTimer.Start(delay);
		return IsDone();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->HouseIndex)
			.Process(this->TargetPos)
			.Process(this->Data) // call FireSuperEntity::Save/Load
			.Process(this->count)
			.Process(this->initDelay)
			.Process(this->initDelayTimer)
			.Process(this->delay)
			.Process(this->delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<FireSuper*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	int count;
	int initDelay;
	CDTimerClass initDelayTimer;
	int delay;
	CDTimerClass delayTimer;
};
