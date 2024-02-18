#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "DestroyAnimData.h"

#include <Ext/TechnoType/DamageText.h>

class DestroyAnim : public StateScript<DestroyAnimData>
{
public:
	STATE_SCRIPT(DestroyAnim, DestroyAnimData);

	HouseClass* pKillerHouse = nullptr;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(pKillerHouse)
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
		return const_cast<DestroyAnim*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
