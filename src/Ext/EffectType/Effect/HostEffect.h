#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <AnimClass.h>

#include "../EffectScript.h"
#include "HostData.h"


/// @brief EffectScript
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class HostEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Host);

	virtual void OnUpdate() override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->_count)
			.Process(this->_isElite)
			.Process(this->_delay)
			.Process(this->_delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<HostEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	GiftBoxEntity GetGiftData()
	{
		if (_isElite && Data->EliteData.Enable)
		{
			return Data->EliteData;
		}
		return Data->Data;
	}

	bool Timeup()
	{
		return _delay <= 0 || _delayTimer.Expired();
	}

	int _count = 0;

	bool _isElite = false;
	int _delay = 0;
	CDTimerClass _delayTimer{};
};
