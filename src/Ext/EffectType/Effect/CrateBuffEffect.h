#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>

#include "../EffectScript.h"
#include "CrateBuffData.h"


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
class CrateBuffEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(CrateBuff);

	virtual void Clean() override
	{
		EffectScript::Clean();

		_updateFlag = false;
	}

	virtual void End(CoordStruct location) override;

	virtual void OnPause() override;

	virtual void OnRecover() override;

	virtual void OnUpdate() override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->_updateFlag)
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
		return const_cast<CrateBuffEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void UpdateStatus();

	bool _updateFlag = false;

};
