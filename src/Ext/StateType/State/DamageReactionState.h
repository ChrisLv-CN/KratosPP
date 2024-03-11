#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "DamageReactionData.h"

class DamageReactionState : public StateScript<DamageReactionData>
{
public:
	STATE_SCRIPT(DamageReaction);

	virtual void OnStart() override;

	virtual void OnUpdate() override;

	virtual void OnReceiveDamage(args_ReceiveDamage* args) override;

	bool ForceDone = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->ForceDone)

			.Process(this->_count)
			.Process(this->_delay)
			.Process(this->_delayTimer)

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
