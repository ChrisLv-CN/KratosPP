#include "TechnoExt.h"

#include <Ext/TechnoStatus.h>
#include <Ext/LaserTrail.h>

void TechnoExt::AddGlobalScripts(std::list<Component*> *globalScripts, ExtData *ext)
{
	TechnoStatus *status = GameCreate<TechnoStatus>(ext);
	// TechnoStatus *status = new TechnoStatus(ext);
	// TechnoStatus status{ ext };
	globalScripts->push_back(status);
	LaserTrail *laserTrail = GameCreate<LaserTrail>(ext);
	// LaserTrail *laserTrail = new LaserTrail(ext);
	// LaserTrail laserTrail{ ext };
	globalScripts->push_back(laserTrail);
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
