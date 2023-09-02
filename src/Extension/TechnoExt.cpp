#include "TechnoExt.h"

void TechnoExt::AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext)
{
	// debug
	std::string msg = std::format("TechenExt ready to create Scripts to ExtData [{}]{}\n", ext->thisID, ext->ownerID);
	Debug::Log(msg.c_str());
	LaserTrail *pLaserTrail = GameCreate<LaserTrail>(ext);
	globalScripts->push_back(pLaserTrail);
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
