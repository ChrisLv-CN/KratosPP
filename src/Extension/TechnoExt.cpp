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
	TechnoStatus* status = new TechnoStatus(ext);
	globalScripts.emplace_back(status);

	// Other Components
	AttachFire<TechnoClass, TechnoExt>* attachFire = new AttachFire<TechnoClass, TechnoExt>(ext);
	globalScripts.emplace_back(attachFire);

	TechnoTrail* trail = new TechnoTrail(ext);
	globalScripts.emplace_back(trail);

	AttachEffect* attachEffect = new AttachEffect(ext);
	globalScripts.emplace_back(attachEffect);
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
std::map<TechnoClass*, bool> TechnoExt::BaseUnitArray{};
std::map<TechnoClass*, bool> TechnoExt::BaseStandArray{};

HealthTextControlData TechnoExt::HealthTextControlData{};
