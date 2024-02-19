#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <AnimClass.h>

#include "../EffectScript.h"
#include "InfoData.h"


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
class InfoEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Info);

	virtual void OnGScreenRenderEnd(CoordStruct location) override;

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
		return const_cast<InfoEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void PrintInfoNumber(int number, ColorStruct houseColor, Point2D pos, InfoEntity data);
	void PrintInfoText(std::string text, ColorStruct houseColor, Point2D pos, InfoEntity data);

	void OffsetAlign(Point2D& pos, std::string text, InfoEntity data);
};
