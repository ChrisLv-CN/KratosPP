#include "AnimExt.h"

#include <Ext/AnimStatus.h>

void AnimExt::AddGlobalScripts(std::list<Component*>* globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// AnimStatus *status = GameCreate<AnimStatus>(ext);
	AnimStatus *status = new AnimStatus(ext);
	globalScripts->push_back(status);
}

AnimExt::ExtContainer AnimExt::ExtMap{};
