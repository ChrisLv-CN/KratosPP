#include "AnimExt.h"

#include <Ext/AnimStatus.h>

void AnimExt::AddGlobalScripts(std::list<Component*>* globalScripts, ExtData* ext)
{
	AnimStatus *status = GameCreate<AnimStatus>(ext);
	globalScripts->push_back(status);
}

AnimExt::ExtContainer AnimExt::ExtMap{};
