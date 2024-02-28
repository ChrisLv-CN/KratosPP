#pragma once

#include <string>
#include <vector>
#include <map>

#include <GeneralDefinitions.h>
#include <WeaponTypeClass.h>

#include <Extension/WeaponTypeExt.h>

#include <Ext/BulletType/BulletStatus.h>

#include "../EffectScript.h"
#include "AttackBeaconData.h"


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
class AttackBeaconEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(AttackBeacon);

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
		EffectScript::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectScript::Save(stream);
		return const_cast<AttackBeaconEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:

	bool RecruitMission(Mission mission);

	int RecruitNumber(std::string typeId, std::map<std::string, int> numbers);

	AttackBeaconEntity GetABData()
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
