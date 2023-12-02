#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/TechnoExt.h>

#include "TrailType/Trail.h"

/// @brief 动态载入组件
class TechnoTrail : public TechnoScript
{
public:
	
	TECHNO_SCRIPT(TechnoTrail);

	void SetupTrails();

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	virtual void OnRemove() override;

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

private:
	bool _setupFlag = false;

	std::vector<Trail> _trails{};
};
