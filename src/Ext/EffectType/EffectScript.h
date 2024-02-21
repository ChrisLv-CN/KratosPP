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

#include "Effect/EffectData.h"
#include "AttachEffectData.h"
#include "AttachEffectScript.h"


#define EFFECT_SCRIPT_BASE(EFFECT_NAME, BASE_NAME) \
	DECLARE_DYNAMIC_SCRIPT(EFFECT_NAME ## Effect, BASE_NAME) \
	\
	virtual EFFECT_NAME ## Data* GetData() override \
	{ \
		return &AEData.EFFECT_NAME; \
	} \
	__declspec(property(get = GetData)) EFFECT_NAME ## Data* Data; \

#define EFFECT_SCRIPT(EFFECT_NAME) \
	EFFECT_SCRIPT_BASE(EFFECT_NAME, EffectScript) \

class EffectScript : public ObjectScript, public IAEScript
{
public:

	virtual void Start() override;
	virtual void Pause() override;
	virtual void Recover() override;

	virtual EffectData* GetData() { return nullptr; };

	AttachEffectData AEData{};

	AttachEffectScript* _ae = nullptr;
	AttachEffectScript* GetAE();
	__declspec(property(get = GetAE)) AttachEffectScript* AE;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->AEData)
			.Process(this->_started)
			.Process(this->_pause)
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
protected:
	bool _started = false;
	bool _pause = false;
};
