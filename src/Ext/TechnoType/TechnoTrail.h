#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Ext/TrailType/Trail.h>

/// @brief 动态载入组件
class TechnoTrail : public TechnoScript
{
public:

	TECHNO_SCRIPT(TechnoTrail);

	virtual void Clean() override
	{
		TechnoScript::Clean();

		_setupFlag = false;
		_trails.clear();
	}

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	virtual void OnRemove() override;

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

private:
	bool _setupFlag = false;

	std::vector<Trail> _trails{};

	void SetupTrails();
};
