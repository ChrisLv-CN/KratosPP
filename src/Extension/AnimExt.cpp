#include "AnimExt.h"

#include <Ext/AnimType/AnimStatus.h>

void AnimExt::AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext)
{
	// Base Component
	globalScripts.push_back(AnimStatus::ScriptName);
}

AnimExt::ExtContainer AnimExt::ExtMap{};
