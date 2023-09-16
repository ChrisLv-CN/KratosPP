#include "TechnoExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/TechnoStatus.h>
#include <Ext/LaserTrail.h>

void TechnoExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// TechnoStatus *status = GameCreate<TechnoStatus>(ext);
	TechnoStatus* status = new TechnoStatus(ext);
	globalScripts.emplace_back(status);

	// Other Components
	LaserTrail* laserTrail = new LaserTrail(ext);
	globalScripts.emplace_back(laserTrail);

	AttachEffect* attachEffect = new AttachEffect(ext);
	globalScripts.emplace_back(attachEffect);
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
