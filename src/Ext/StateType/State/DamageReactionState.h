#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "DamageReactionData.h"

class DamageReactionState : public StateScript<DamageReactionData>
{
public:
	STATE_SCRIPT(DamageReaction);

	virtual void Clean() override
	{
		StateScript<DamageReactionData>::Clean();

		ForceDone = false;
		_count = 0;
		_delay = 0;
		_delayTimer = {};

		_animDelay = 0;
		_animDelayTimer = {};

		_isElite = false;
	}

	virtual void OnStart() override;

	virtual void OnUpdate() override;

	virtual void OnReceiveDamage(args_ReceiveDamage* args) override;

	bool ForceDone = false;

	DamageReactionState& operator=(const DamageReactionState& other)
	{
		if (this != &other)
		{
			StateScript<DamageReactionData>::operator=(other);
			ForceDone = other.ForceDone;
			_count = other._count;
			_delay = other._delay;
			_delayTimer = other._delayTimer;
			_animDelay = other._animDelay;
			_animDelayTimer = other._animDelayTimer;
			_isElite = other._isElite;
		}
		return *this;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->ForceDone)

			.Process(this->_count)
			.Process(this->_delay)
			.Process(this->_delayTimer)

			.Process(this->_animDelay)
			.Process(this->_animDelayTimer)

			.Process(this->_isElite)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<DamageReactionData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<DamageReactionData>::Save(stream);
		return const_cast<DamageReactionState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	DamageReactionEntity GetDataEntity();

	void ActionOnce();

	bool IsReady();

	bool Timeup();

	bool IsDone();

	bool CanPlayAnim();

	int _count = 0;
	int _delay = 0;
	CDTimerClass _delayTimer{};

	int _animDelay = 0;
	CDTimerClass _animDelayTimer{};

	bool _isElite = false;
};
