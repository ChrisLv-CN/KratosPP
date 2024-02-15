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


#define EFFECT_SCRIPT(CLASS_NAME, EFFECT_DATA) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, EffectScript) \
	\
	EFFECT_DATA GetData() \
	{ \
		return AEData.CLASS_NAME; \
	} \
	__declspec(property(get = GetData)) EFFECT_DATA Data; \

class EffectScript : public ObjectScript, public IAEScript
{
public:
	OBJECT_SCRIPT(EffectScript);

	std::string Token{ "" };
	AttachEffectData AEData{};

	AttachEffectScript* _ae = nullptr;
	AttachEffectScript* GetAE();
	__declspec(property(get = GetAE)) AttachEffectScript* AE;

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
