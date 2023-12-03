#include "TechnoExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/TechnoStatus.h>

void TechnoExt::AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext)
{
	// Base Component
	globalScripts.push_back(TechnoStatus::ScriptName);
	globalScripts.push_back(AttachEffect::ScriptName);
}

void TechnoExt::ClearBaseArray(EventSystem* sender, Event e, void* args)
{
	BaseUnitArray.clear();
	BaseStandArray.clear();
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
std::map<TechnoClass*, bool> TechnoExt::BaseUnitArray{};
std::map<TechnoClass*, bool> TechnoExt::BaseStandArray{};

HealthTextControlData TechnoExt::HealthTextControlData{};
