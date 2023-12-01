#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/BulletExt.h>

#include "TrailType/Trail.h"


class BulletTrail : public BulletScript
{
public:
	BULLET_SCRIPT(BulletTrail);

	virtual void Awake() override;

	virtual void OnUpdateEnd() override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->trails)
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
		return const_cast<BulletTrail*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	std::vector<Trail> trails{};
};
