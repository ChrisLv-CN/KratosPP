#include "EBoltExt.h"

#include <Ext/EBoltStatus.h>

void EBoltExt::AddGlobalScripts(std::list<Component*>* globalScripts, ExtData* ext)
{
	EBoltStatus *status = GameCreate<EBoltStatus>(ext);
	globalScripts->push_back(status);
}

EBoltExt::ExtContainer EBoltExt::ExtMap{};
