#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <type_traits>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

class AttachEffect : public ObjectScript
{
public:
	OBJECT_SCRIPT(AttachEffect);


#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
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
		return const_cast<AttachEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
