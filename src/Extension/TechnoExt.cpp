#include "TechnoExt.h"

void TechnoExt::AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext)
{
	LaserTrail *pLaserTrail = GameCreate<LaserTrail>(ext);
	globalScripts->push_back(pLaserTrail);
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
