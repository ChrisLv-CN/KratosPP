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

#include "AttachEffectData.h"
#include "AttachEffectScript.h"


#define EFFECT_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, EffectScript) \

#define EFFECT_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \


class EffectScript : public ObjectScript
{
public:
	OBJECT_SCRIPT(EffectScript);

	std::string Token{ "" };
	AttachEffectData AEData{};

	AttachEffectScript* _ae = nullptr;
	AttachEffectScript* GetAE();
	__declspec(property(get = GetAE)) AttachEffectScript* AE;

	virtual void ResetDuration() {};

	virtual void Enable() {};
	virtual void Disable(CoordStruct location) {};

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->Token)
			.Process(this->AEData)
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
		return const_cast<EffectScript*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
