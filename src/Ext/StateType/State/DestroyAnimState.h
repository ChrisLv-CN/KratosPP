#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "DestroyAnimData.h"

#include <Ext/TechnoType/DamageText.h>

class DestroyAnimState : public StateScript<DestroyAnimData>
{
public:
	STATE_SCRIPT(DestroyAnim);

	HouseClass* pKillerHouse = nullptr;

	DestroyAnimState& operator=(const DestroyAnimState& other)
	{
		if (this != &other)
		{
			StateScript<DestroyAnimData>::operator=(other);
			pKillerHouse = other.pKillerHouse;
		}
		return *this;
	}
#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pKillerHouse)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<DestroyAnimData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<DestroyAnimData>::Save(stream);
		return const_cast<DestroyAnimState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
