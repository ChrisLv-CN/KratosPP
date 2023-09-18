#pragma once

#include <Windows.h>
#include <string>
#include <format>
#include <codecvt>

#include <Extension.h>
#include <TechnoClass.h>
#include <Matrix3D.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

enum class DrivingState
{
	Moving = 0, Stand = 1, Start = 2, Stop = 3
};

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	TechnoStatus(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();

		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	virtual void Destroy() override
	{
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	void DrawINFO(EventSystem* sender, Event e, void* args)
	{
		if (args)
		{
#ifdef DEBUG
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conver;
			std::wstring text = std::format(L"{} {} {}", conver.from_bytes(extId), conver.from_bytes(thisId), conver.from_bytes(baseId));
			Point2D pos{};
			CoordStruct location = _owner->GetCoords();
			TacticalClass::Instance->CoordsToClient(location, &pos);
			DSurface::Temp->DrawText(text.c_str(), &pos, Drawing::RGB_To_Int(Drawing::TooltipColor.get()));
#endif // DEBUG
		}
	}

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

	DrivingState drivingState = DrivingState::Moving;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<TechnoStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	Mission _lastMission = Mission::Guard;
};
