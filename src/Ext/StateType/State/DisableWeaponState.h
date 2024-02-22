#pragma once

#include "../StateScript.h"
#include "DisableWeaponData.h"

class DisableWeaponState : public StateScript<DisableWeaponData>
{
public:
	STATE_SCRIPT(DisableWeapon);

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<DisableWeaponData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<DisableWeaponData>::Save(stream);
		return const_cast<DisableWeaponState*>(this)->Serialize(stream);
	}
#pragma endregion
};
