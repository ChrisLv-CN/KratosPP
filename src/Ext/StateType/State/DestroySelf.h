#pragma once

#include "../StateScript.h"
#include "DestroySelfData.h"

#include <Ext/TechnoType/DamageText.h>

class DestroySelf : public StateScript<DestroySelfData>
{
public:
	STATE_SCRIPT(DestroySelf, DestroySelfData);

	bool AmIDead();

	void DestroyNow(bool peaceful);

	virtual void OnStart() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->GoDie)
			.Process(this->_delay)
			.Process(this->_delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<DestroySelfData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<DestroySelfData>::Save(stream);
		return const_cast<DestroySelf*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void ExtendLife();

	bool Timeup();

	bool GoDie = false;

	int _delay = 0;
	CDTimerClass _delayTimer{};
};
