#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "TeleportData.h"

#include <Ext/TechnoType/DamageText.h>

class TeleportState : public StateScript<TeleportData>
{
public:
	enum class TeleportStep
	{
		NONE = 0, READY = 1, TELEPORTED = 2, FREEZING = 3, MOVEFORWARD = 4
	};

	STATE_SCRIPT(Teleport);

	TeleportState& operator=(const TeleportState& other)
	{
		if (this != &other)
		{
			StateScript<TeleportData>::operator=(other);
			_count = other._count;
			_delay = other._delay;
			_delayTimer = other._delayTimer;
		}
		return *this;
	}

	bool Teleport(CoordStruct* pLocation, WarheadTypeClass* pWH);

	virtual void OnStart() override;

	virtual void OnUpdate() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_count)
			.Process(this->_delay)
			.Process(this->_delayTimer)

			.Process(this->_step)
			.Process(this->_warpTo)
			.Process(this->_loco)
			.Process(this->_teleportTimer)
			.Process(this->pDest)
			.Process(this->pFocus)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<TeleportData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<TeleportData>::Save(stream);
		return const_cast<TeleportState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void Reload();

	bool IsReady();

	bool Timeup();

	bool IsDone();

	int _count = 0;
	int _delay = 0;
	CDTimerClass _delayTimer{};

	CoordStruct GetAndMarkDestination();

	TeleportStep _step = TeleportStep::READY;

	CoordStruct _warpTo; // 弹头传进来的坐标
	ILocomotion* _loco = nullptr; // 原始的loco
	CDTimerClass _teleportTimer{}; // 传送冰冻时间

	AbstractClass* pDest = nullptr;
	AbstractClass* pFocus = nullptr;
};
