﻿#pragma once

#include "../StateScript.h"
#include "OverrideWeaponData.h"

class OverrideWeaponState : public StateScript<OverrideWeaponData>
{
public:
	STATE_SCRIPT(OverrideWeapon);

	bool TryGetOverrideWeapon(bool isElite, bool isDeadWeapon, WeaponTypeClass*& pOverrideWeapon);

	virtual void Clean() override
	{
		StateScript<OverrideWeaponData>::Clean();

		_weaponIndex = -1;
	}

	virtual void OnFire(AbstractClass* pTarget, int weaponIndex) override;

	OverrideWeaponState& operator=(const OverrideWeaponState& other)
	{
		if (this != &other)
		{
			StateScript<OverrideWeaponData>::operator=(other);
			_weaponIndex = other._weaponIndex;
		}
		return *this;
	}
#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_weaponIndex)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<OverrideWeaponData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<OverrideWeaponData>::Save(stream);
		return const_cast<OverrideWeaponState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool CanOverride(bool isElite, bool isDeadWeapon, std::string& weaponId);

	int _weaponIndex = -1;
};
