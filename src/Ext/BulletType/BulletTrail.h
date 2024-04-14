#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Ext/TrailType/Trail.h>


class BulletTrail : public BulletScript
{
public:
	BULLET_SCRIPT(BulletTrail);

	void SetupTrails();

	virtual void Clean() override
	{
		BulletScript::Clean();

		_setupFlag = false;
		_trails.clear();
	}

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

private:
	bool _setupFlag = false;

	std::vector<Trail> _trails{};
};
