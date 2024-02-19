#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "FireSuperData.h"

#include <Ext/TechnoType/DamageText.h>

class FireSuperState : public StateScript<FireSuperData>
{
public:
	STATE_SCRIPT(FireSuper);

	virtual void Awake() override
	{
		StateScript<FireSuperData>::Awake();
		Tag = GetUUID();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<FireSuperData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<FireSuperData>::Save(stream);
		return const_cast<FireSuperState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
