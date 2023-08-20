#pragma once

#include <Extension.h>
#include <TechnoClass.h>
#include <LaserDrawClass.h>

#include <Common/Components/Component.h>
#include <Extension/GOExtension.h>

class LaserTrail : public Component
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

	virtual void OnUpdate() override
	{

		CoordStruct sourcePos = pOwner->GetCoords();
		CoordStruct targetPos = sourcePos + CoordStruct(1024, 0, 0);
		LaserDrawClass* pLaser = GameCreate<LaserDrawClass>(
			sourcePos, targetPos,
			ColorStruct{ 255, 0,0 }, ColorStruct{ 0, 0, 0 }, ColorStruct{ 0, 0, 0 },
			15);

		pLaser->Thickness = 5;
		pLaser->IsHouseColor = true;
		pLaser->IsSupported = true;

	}
};

