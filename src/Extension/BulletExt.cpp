#include "BulletExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/AttachFire.h>
#include <Ext/BulletStatus.h>
#include <Ext/BulletTrail.h>

void BulletExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// BulletStatus *status = GameCreate<BulletStatus>(ext);
	BulletStatus* status = CREATE_SCRIPT<BulletStatus>("BulletStatus", ext);
	globalScripts.emplace_back(status);

	// Other Component
	AttachFire* attachFire = CREATE_SCRIPT<AttachFire>("AttachFire", ext);
	globalScripts.emplace_back(attachFire);

	BulletTrail* trail = CREATE_SCRIPT<BulletTrail>("BulletTrail", ext);
	globalScripts.emplace_back(trail);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
std::vector<BulletClass*> BulletExt::TargetAircraftBullets{};
