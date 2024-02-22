#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "AntiBulletData.h"

#include <Ext/TechnoType/DamageText.h>

class AntiBulletState : public StateScript<AntiBulletData>
{
public:
	STATE_SCRIPT(AntiBullet);

	bool CanSearchBullet();

	AntiBulletState& operator=(const AntiBulletState& other)
	{
		if (this != &other)
		{
			StateScript<AntiBulletData>::operator=(other);
			_delayTimer = other._delayTimer;
		}
		return *this;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<AntiBulletData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<AntiBulletData>::Save(stream);
		return const_cast<AntiBulletState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	CDTimerClass _delayTimer{};
};
