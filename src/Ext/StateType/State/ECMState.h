#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "ECMData.h"

#include <Ext/TechnoType/DamageText.h>

class BulletStatus;

class ECMState : public StateScript<ECMData>
{
public:
	STATE_SCRIPT(ECM);

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

			.Process(this->_lockTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<ECMData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<ECMData>::Save(stream);
		return const_cast<ECMState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool TryGetSourceLocation(CoordStruct& location);

	void Reload();

	bool IsReady();

	bool Timeup();

	bool IsDone();

	int _count = 0;
	int _delay = 0;
	CDTimerClass _delayTimer{};

	CDTimerClass _lockTimer{};

	BulletStatus* _status = nullptr;
	BulletStatus* GetBulletStatus();
};
