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

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	TechnoStatus(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();

		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	virtual void Awake() override
	{
#ifdef DEBUG_COMPONENT
		Debug::Log("Component [%s]%s calling Awake.\n", thisName.c_str(), thisId.c_str());
#endif // DEBUG
	}

	virtual void Start() override
	{
#ifdef DEBUG_COMPONENT
		Debug::Log("Component [%s]%s calling Start.\n", thisName.c_str(), thisId.c_str());
#endif // DEBUG
	}

	virtual void Destroy() override
	{
#ifdef DEBUG_COMPONENT
		Debug::Log("Component [%s]%s calling Destroy.\n", thisName.c_str(), thisId.c_str());
#endif // DEBUG
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

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		return const_cast<TechnoStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
};
