#pragma once

#include "../StateScript.h"
#include "FreezeData.h"

#include <Ext/TechnoType/DamageText.h>

class FreezeState : public StateScript<FreezeData>
{
public:
	STATE_SCRIPT(Freeze);

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<FreezeData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<FreezeData>::Save(stream);
		return const_cast<FreezeState*>(this)->Serialize(stream);
	}
#pragma endregion
};
