#include "BulletExt.h"

#include <Ext/BulletStatus.h>
#include <Ext/BulletTrail.h>

void BulletExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// BulletStatus *status = GameCreate<BulletStatus>(ext);
	BulletStatus* status = new BulletStatus(ext);
	globalScripts.push_back(status);

	// Other Component
	BulletTrail* trail = new BulletTrail(ext);
	globalScripts.push_back(trail);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
