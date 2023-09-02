#pragma once

#include <codecvt>
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
#include <Extension/TechnoExt.h>

class LaserTrailData : public INIConfig<LaserTrailData>
{
public:
	virtual void Read(INI_EX ini) override
	{
	}

	Valueable<bool> ColorChanged{false};
};

class LaserTrail : public TechnoScript
{
public:
	std::string thisID;

	LaserTrail(Extension<TechnoClass> *ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();
		char t[1024];
		sprintf_s(t, "%p", this);
		this->thisID.assign(t);
		std::string msg = std::format("LaserTrail[{}] is create for ExtData [{}]{}\n", thisID, ext->thisID, ext->ownerID);
		Debug::Log(msg.c_str());
	}

	virtual void Awake() override
	{
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &LaserTrail::DrawINFO);
		/*
		std::string adjacent = INI::GetSection(INI::Rules, _owner->GetType()->ID)->Get<std::string>("Adjacent", "0");
		Debug::Log("Adjacent = %s\n", adjacent.c_str());
		// test
		CoordStruct offset = INI::GetSection(INI::Rules, "MarkTest")->Get<CoordStruct>("Paintball.Color", CoordStruct::Empty);
		Debug::Log("Paintball.Color = {%d, %d, %d}\n", offset.X, offset.Y, offset.Z);
		std::string section = "EnemyDefenseSpecial";
		std::string key = "Deliver.Types";
		std::vector<std::string> def;
		std::vector<std::string> vals = INI::GetSection(INI::Rules, section.c_str())->GetList<std::string>(key.c_str(), def);
		std::string logMsg = "[EnemyDefenseSpecial]\nDeliver.Types=";
		for (std::string s : vals)
		{
			logMsg.append(s).append(", ");
		}
		logMsg.append("\n");
		Debug::Log(logMsg.c_str());
		*/
		Debug::Log("[%s] Awake data\n LaserColor = {%d, %d, %d}\n ColorChanged = %d\n", thisID.c_str(), laserColor.R, laserColor.G, laserColor.B, colorChanged);
	}

	virtual void Destroy() override
	{
		std::string msg = std::format("LaserTrail[{}] is delete for ExtData [{}]{}\n", thisID, ExtData->thisID, ExtData->ownerID);
		Debug::Log(msg.c_str());
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &LaserTrail::DrawINFO);
	}

	template<typename T>
	void Serialize(T &stream)
	{
		stream.Process(this->laserColor).Process(this->colorChanged);
	};

	virtual void LoadFromStream(ExStreamReader &stream) override
	{
		Component::LoadFromStream(stream);
		this->Serialize(stream);
	}

	virtual void SaveToStream(ExStreamWriter &stream) override
	{
		Component::SaveToStream(stream);
		this->Serialize(stream);
	}

	void DrawINFO(EventSystem *sender, Event e, void *args)
	{
		if (args)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conver;
			std::wstring text = std::format(L"{} {}", conver.from_bytes(thisID), std::to_wstring(colorChanged));
			Point2D pos{};
			CoordStruct location = _owner->GetCoords();
			TacticalClass::Instance->CoordsToClient(location, &pos);
			DSurface::Temp->DrawText(text.c_str(), &pos, Drawing::RGB_To_Int(Drawing::TooltipColor.get()));
		}
	}

	ColorStruct laserColor = {0, 255, 0};

	bool colorChanged = false;

	virtual void OnInit() override
	{
		Debug::Log("[%s] OnInit\n LaserColor = {%d, %d, %d}\n ColorChanged = %d\n", thisID.c_str(), laserColor.R, laserColor.G, laserColor.B, colorChanged);
	}

	virtual void OnUpdate() override
	{
		INI::GetSection(INI::Rules, this->_owner->GetType()->get_ID());
		if (_owner->IsSelected)
		{
			laserColor = ColorStruct{0, 0, 255};
			colorChanged = true;
		}
		CoordStruct sourcePos = _owner->GetCoords();
		CoordStruct targetPos = sourcePos + CoordStruct(0, 0, 1024);
		LaserDrawClass *pLaser = GameCreate<LaserDrawClass>(
			sourcePos, targetPos,
			laserColor, ColorStruct{0, 0, 0}, ColorStruct{0, 0, 0},
			2);

		pLaser->Thickness = 5;
		pLaser->IsHouseColor = true;
		pLaser->IsSupported = true;
	}

private:
	LaserTrailData _data;
};
