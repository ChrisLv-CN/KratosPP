#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/TechnoExt.h>

#include "TrailType/Trail.h"


class TechnoTrail : public TransformScript
{
public:
	TechnoTrail(TechnoExt::ExtData* ext) : TransformScript(ext)
	{
		this->Name = typeid(this).name();
	}

	void SetupTrails();

	virtual bool OnAwake() override;

	virtual void OnTransform(TypeChangeEventArgs* args) override;

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	virtual void OnRemove() override;

	virtual void OnUpdateEnd() override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->trails)
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
		return const_cast<TechnoTrail*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	std::vector<Trail> trails{};
};
