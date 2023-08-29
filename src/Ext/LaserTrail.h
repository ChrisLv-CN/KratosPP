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
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Extension/GOExtension.h>
#include <Extension/TechnoTypeExt.h>

class LaserTrailData : public INIConfig<LaserTrailData>
{
public:
	virtual void Read(INI_EX ini) override
	{ }

	Valueable<bool> ColorChanged{ false };
};

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

	virtual void Serialize(StreamWorkerBase& stream) override
	{
		Debug::Log("********Serialize*******\n");
		stream.Process(this->laserColor)->Process(this->colorChanged);
	};

	void DrawINFO(EventSystem* sender, Event e, void* args)
	{
		if (args)
		{
			std::wstring text = std::to_wstring(i);
			text.append(L"\n").append(std::to_wstring(colorChanged));
			Point2D pos{};
			CoordStruct location = _owner->GetCoords();
			TacticalClass::Instance->CoordsToClient(location, &pos);
			DSurface::Temp->DrawText(text.c_str(), &pos, Drawing::RGB_To_Int(Drawing::TooltipColor.get()));
		}
	}

	Valueable<ColorStruct> laserColor{ { 0,255,0 } };

	bool colorChanged = false;

	virtual void Awake() override
	{
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &LaserTrail::DrawINFO);

		INI::GetSection(INI::Rules, _owner->GetType()->ID);
	}

	virtual void OnUpdate() override
	{
		INI::GetSection(INI::Rules, this->_owner->GetType()->get_ID());
		if (_owner->IsSelected)
		{
			laserColor = ColorStruct{ 0, 0, 255 };
			colorChanged = true;
		}
		CoordStruct sourcePos = _owner->GetCoords();
		CoordStruct targetPos = sourcePos + CoordStruct(0, 0, 1024);
		LaserDrawClass* pLaser = GameCreate<LaserDrawClass>(
			sourcePos, targetPos,
			laserColor, ColorStruct{ 0, 0, 0 }, ColorStruct{ 0, 0, 0 },
			2);

		pLaser->Thickness = 5;
		pLaser->IsHouseColor = true;
		pLaser->IsSupported = true;
	}

private:
	LaserTrailData _data;
};

