#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "TransformData.h"

#include <Ext/TechnoType/DamageText.h>

class TransformState : public StateScript<TransformData>
{
public:
	STATE_SCRIPT(Transform);

	virtual void Clean() override
	{
		StateScript<TransformData>::Clean();
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
		StateScript<TransformData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<TransformData>::Save(stream);
		return const_cast<TransformState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
