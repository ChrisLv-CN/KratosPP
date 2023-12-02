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

	void SetupTrails();

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

private:
	bool _setupFlag = false;

	std::vector<Trail> _trails{};
};
