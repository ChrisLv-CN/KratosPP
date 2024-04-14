#pragma once

#include "../StateScript.h"
#include "DeselectData.h"

#include <Ext/TechnoType/DamageText.h>

class DeselectState : public StateScript<DeselectData>
{
public:
	STATE_SCRIPT(Deselect);

	virtual void Clean() override
	{
		StateScript<DeselectData>::Clean();
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
		StateScript<DeselectData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<DeselectData>::Save(stream);
		return const_cast<DeselectState*>(this)->Serialize(stream);
	}
#pragma endregion
};
