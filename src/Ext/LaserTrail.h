#pragma once

#include <Extension.h>
#include <TechnoClass.h>
#include <LaserDrawClass.h>
#include <Drawing.h>
#include <HouseClass.h>
#include <TacticalClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Extension/GOExtension.h>

class LaserTrail : public TechnoScript
{
public:
	static int id;
	int i;

	LaserTrail(Extension<TechnoClass>* ext) : TechnoScript(ext)
	{
		id++;
		i = id;
	}

	virtual void OnDestroy() override
	{
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &LaserTrail::DrawINFO);
	}

	void DrawINFO(EventSystem* sender, Event e, void* args)
	{
		if (args)
		{
			std::wstring text = std::to_wstring(i);
			Point2D pos{};
			CoordStruct location = _Owner->GetCoords();
			TacticalClass::Instance->CoordsToClient(location, &pos);
			DSurface::Temp->DrawText(text.c_str(), &pos, Drawing::RGB_To_Int(Drawing::TooltipColor.get()));
		}
	}

	ColorStruct laserColor{ 0,255,0 };

	virtual void Awake() override
	{
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &LaserTrail::DrawINFO);
	}

	virtual void OnUpdate() override
	{
		if (_Owner->IsSelected)
		{
			laserColor = { 0, 0, 255 };
		}
		CoordStruct sourcePos = _Owner->GetCoords();
		CoordStruct targetPos = sourcePos + CoordStruct(0, 0, 1024);
		LaserDrawClass* pLaser = GameCreate<LaserDrawClass>(
			sourcePos, targetPos,
			laserColor, ColorStruct{ 0, 0, 0 }, ColorStruct{ 0, 0, 0 },
			2);

		pLaser->Thickness = 5;
		pLaser->IsHouseColor = true;
		pLaser->IsSupported = true;
	}
};

