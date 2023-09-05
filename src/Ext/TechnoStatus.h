#pragma once

#include <string>
#include <format>
#include <codecvt>

#include <Extension.h>
#include <TechnoClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Extension/TechnoExt.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	std::string thisID{};
	std::string extID{};
	std::string technoID{};

	TechnoStatus(Extension<TechnoClass> *ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();

		char t[1024];
		sprintf_s(t, "%p", this);
		thisID = {t};

		char tt[1024];
		sprintf_s(tt, "%p", ext);
		extID = {tt};

		char ttt[1024];
		sprintf_s(ttt, "%p", ext->OwnerObject());
		technoID = {ttt};
	}

	virtual void Awake() override
	{
#ifdef DEBUG
		const char *typeId = "Unknow";
		if (_owner->GetTechnoType())
		{
			typeId = _owner->GetTechnoType()->ID;
		}
		Debug::Log("Techno [%s]%d calling TechnoStatus::Awake to init data.\n", typeId, _owner);
#endif
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	virtual void Destroy() override
	{
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	void DrawINFO(EventSystem *sender, Event e, void *args)
	{
		if (args)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conver;
			std::wstring text = std::format(L"{} {} {}", conver.from_bytes(extID), conver.from_bytes(thisID), conver.from_bytes(technoID));
			Point2D pos{};
			CoordStruct location = _owner->GetCoords();
			TacticalClass::Instance->CoordsToClient(location, &pos);
			DSurface::Temp->DrawText(text.c_str(), &pos, Drawing::RGB_To_Int(Drawing::TooltipColor.get()));
		}
	}

#pragma region save/load
	template <typename T>
	void Serialize(T &stream)
	{ };

	virtual void LoadFromStream(ExStreamReader &stream) override
	{
#ifdef DEBUG
		const char *typeId = "Unknow";
		if (_owner->GetType())
		{
			typeId = _owner->GetType()->ID;
		}
		Debug::Log("Techno [%s]%d calling TechnoStatus::Load to Serialize data.\n", typeId, _owner);
#endif
		Component::LoadFromStream(stream);
		this->Serialize(stream);
	}
	virtual void SaveToStream(ExStreamWriter &stream) override
	{
		Component::SaveToStream(stream);
		this->Serialize(stream);
	}
#pragma endregion

private:
};

// Helper
static bool IsDead(TechnoClass *pTechno)
{
	return !pTechno || !pTechno->GetType() || pTechno->Health <= 0 || !pTechno->IsAlive || pTechno->IsCrashing || pTechno->IsSinking;
}
