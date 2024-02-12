#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <AnimClass.h>

#include "../EffectScript.h"
#include "AnimationData.h"


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
class Animation : public EffectScript
{
public:
	EFFECT_SCRIPT(Animation);

	AnimationData GetData()
	{
		return AEData.Animation;
	}
	__declspec(property(get = GetData)) AnimationData Data;

	void UpdateLocationOffset(CoordStruct offset);

	virtual void Enable() override;

	virtual void Disable(CoordStruct location) override;

	virtual void OnPut(CoordStruct* pCoord, DirType faceDir) override;
	virtual void OnUpdate() override;
	virtual void OnRemove() override;
	virtual void OnReceiveDamage(args_ReceiveDamage* args) override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->pAnim)
			.Process(this->animFlags)
			.Process(this->ownerIsCloak)
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
		return const_cast<Animation*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	AnimClass* pAnim = nullptr;
	BlitterFlags animFlags = BlitterFlags::None;
	bool ownerIsCloak = false;

	void CreateIdleAnim(bool force = false, CoordStruct location = CoordStruct::Empty);

	void KillIdleAnim();

};
