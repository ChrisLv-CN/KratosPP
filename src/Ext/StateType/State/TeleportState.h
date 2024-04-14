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

	bool Teleport(CoordStruct* pLocation, WarheadTypeClass* pWH);

	bool IsFreezing();

	bool IsReadyToMoveWarp();

	virtual void Clean() override
	{
		StateScript<TeleportData>::Clean();

		_count = 0;
		_delay = 0;
		_delayTimer = {};

		// 状态机一直处于激活状态，额外开关控制是否可以进行传送
		_canWarp = false;
		_step = TeleportStep::READY;

		_warpTo; // 弹头传进来的坐标
		_teleportTimer = {}; // 传送冰冻时间

		pDest = nullptr;
		pFocus = nullptr;
	}

	virtual void Deactivate() override
	{
		// 永久激活，不可关闭
	}

	virtual void OnStart() override;

	virtual void OnEnd() override;

	virtual void OnUpdate() override;

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

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_count)
			.Process(this->_delay)
			.Process(this->_delayTimer)

			.Process(this->_canWarp)
			.Process(this->_step)
			.Process(this->_warpTo)
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
	CoordStruct GetAndMarkDestination(CoordStruct location);

	void Reload();

	bool IsReady();

	bool Timeup();

	bool IsDone();

	int _count = 0;
	int _delay = 0;
	CDTimerClass _delayTimer{};

	// 状态机一直处于激活状态，额外开关控制是否可以进行传送
	bool _canWarp = false;
	TeleportStep _step = TeleportStep::READY;

	CoordStruct _warpTo; // 弹头传进来的坐标
	CDTimerClass _teleportTimer{}; // 传送冰冻时间

	AbstractClass* pDest = nullptr;
	AbstractClass* pFocus = nullptr;
};
