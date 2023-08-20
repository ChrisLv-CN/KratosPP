#pragma once

#include <Extension.h>
#include <TechnoClass.h>
#include <LaserDrawClass.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Extension/GOExtension.h>

class LaserTrail : public ScriptComponent
{
private:
	TechnoClass* pOwner = nullptr;
public:
	LaserTrail()
	{

	}
	LaserTrail(TechnoClass* OwnerObject)
	{
		pOwner = OwnerObject;
	}

	ColorStruct laserColor{ 0,255,0 };
	int i = 0;

	virtual void OnUpdate() override
	{
		if (pOwner->IsSelected)
		{
			laserColor = { 0, 0, 255 };
			i++;
		}
		CoordStruct sourcePos = pOwner->GetCoords();
		CoordStruct targetPos = sourcePos + CoordStruct(1024, 0, 0);
		LaserDrawClass* pLaser = GameCreate<LaserDrawClass>(
			sourcePos, targetPos,
			laserColor, ColorStruct{ 0, 0, 0 }, ColorStruct{ 0, 0, 0 },
			15);

		pLaser->Thickness = 5;
		pLaser->IsHouseColor = true;
		pLaser->IsSupported = true;
	}
};

