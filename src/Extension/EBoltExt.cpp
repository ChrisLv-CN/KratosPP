#include "EBoltExt.h"

#include <Ext/EBoltType/EBoltStatus.h>

void EBoltExt::AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext)
{
	// Base Component
	globalScripts.push_back(EBoltStatus::ScriptName);
}

EBoltExt::ExtContainer EBoltExt::ExtMap{};
