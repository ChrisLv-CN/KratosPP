#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <SpecificStructures.h>

#include "../EffectScript.h"
#include "ImmuneData.h"


/// @brief EffectScript
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class ImmuneEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Immune);

	virtual void OnUpdate() override;

	virtual void OnReceiveDamage(args_ReceiveDamage* args) override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		EffectScript::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectScript::Save(stream);
		return const_cast<ImmuneEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void ImmuneLogic();

};
