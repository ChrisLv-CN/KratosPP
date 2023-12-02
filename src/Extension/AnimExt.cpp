#include "AnimExt.h"

#include <Ext/AnimStatus.h>

void AnimExt::AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext)
{
	// Base Component
	globalScripts.push_back(AnimStatus::ScriptName);
}

AnimExt::ExtContainer AnimExt::ExtMap{};
