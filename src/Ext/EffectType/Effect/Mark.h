#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <AnimClass.h>

#include "../EffectScript.h"
#include "MarkData.h"


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
class Mark : public EffectScript
{
public:
	EFFECT_SCRIPT(Mark);

	MarkData GetData()
	{
		return AEData.Mark;
	}
	__declspec(property(get = GetData)) MarkData Data;

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
		return const_cast<Mark*>(this)->Serialize(stream);
	}
#pragma endregion
private:

};
