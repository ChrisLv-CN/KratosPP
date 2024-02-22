#pragma once

#include <string>
#include <vector>
#include <map>

#include <GeneralDefinitions.h>

#include <Extension/WeaponTypeExt.h>

#include "../EffectScript.h"
#include "ExtraFireData.h"


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
class ExtraFireEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(ExtraFire);

	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->_rof)
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
		return const_cast<ExtraFireEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool CheckROF(WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData* weaponData);

	void ResetROF(WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData* weaponData,double rofMultip);

	std::map<std::string, CDTimerClass> _rof{};
};
