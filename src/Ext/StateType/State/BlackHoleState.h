#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "BlackHoleData.h"

class BlackHoleState : public StateScript<BlackHoleData>
{
public:
	STATE_SCRIPT(BlackHole);

	bool IsOutOfRange(CoordStruct targetPos);

	virtual void OnStart() override;

	virtual void OnUpdate() override;

	BlackHoleState& operator=(const BlackHoleState& other)
	{
		if (this != &other)
		{
			StateScript<BlackHoleData>::operator=(other);
			_count = other._count;
			_delay = other._delay;
			_delayTimer = other._delayTimer;
			_isElite = other._isElite;
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

			.Process(this->_isElite)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<BlackHoleData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<BlackHoleData>::Save(stream);
		return const_cast<BlackHoleState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void StartCapture();

	BlackHoleEntity GetDataEntity();

	void Reload();

	bool IsReady();

	bool Timeup();

	bool IsDone();

	int _count = 0;
	int _delay = 0;
	CDTimerClass _delayTimer{};

	bool _isElite = false;
};
