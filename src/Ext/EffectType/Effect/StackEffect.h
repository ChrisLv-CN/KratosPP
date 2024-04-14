#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <AnimClass.h>

#include "../EffectScript.h"
#include "StackData.h"


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
class StackEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Stack);

	virtual void Clean() override
	{
		EffectScript::Clean();

		_count = 0;
	}

	virtual void OnUpdate() override;
	virtual void OnWarpUpdate() override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->_count)
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
		return const_cast<StackEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void Watch();
	bool CanActive(int stacks, int level, Condition condition);

	int _count = 0;
};
