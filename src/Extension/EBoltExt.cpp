#include "EBoltExt.h"

#include <Ext/EBoltStatus.h>

void EBoltExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// EBoltStatus *status = GameCreate<EBoltStatus>(ext);
	EBoltStatus* status = new EBoltStatus(ext);
	globalScripts.push_back(status);
}

EBoltExt::ExtContainer EBoltExt::ExtMap{};
