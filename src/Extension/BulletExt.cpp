#include "BulletExt.h"

#include <Ext/BulletStatus.h>

void BulletExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// BulletStatus *status = GameCreate<BulletStatus>(ext);
	BulletStatus* status = new BulletStatus(ext);
	globalScripts.push_back(status);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
