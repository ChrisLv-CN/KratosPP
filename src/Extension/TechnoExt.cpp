#include "TechnoExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/AttachFire.h>
#include <Ext/TechnoStatus.h>
#include <Ext/TechnoTrail.h>

void TechnoExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// TechnoStatus *status = GameCreate<TechnoStatus>(ext);
	// TechnoStatus* status = new TechnoStatus(ext);
	TechnoStatus* status = CREATE_SCRIPT<TechnoStatus>("TechnoStatus", ext);
	globalScripts.emplace_back(status);

	AttachEffect* attachEffect = CREATE_SCRIPT<AttachEffect>("AttachEffect", ext);
	globalScripts.emplace_back(attachEffect);

	// Other Components
	AttachFire* attachFire = CREATE_SCRIPT<AttachFire>("AttachFire", ext);
	globalScripts.emplace_back(attachFire);

	// TechnoTrail* trail = new TechnoTrail(ext);
	TechnoTrail* trail = CREATE_SCRIPT<TechnoTrail>("TechnoTrail", ext);
	globalScripts.emplace_back(trail);

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
