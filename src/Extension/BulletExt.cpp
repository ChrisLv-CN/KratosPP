﻿#include "BulletExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/AttachFire.h>
#include <Ext/BulletStatus.h>
#include <Ext/BulletTrail.h>

void BulletExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// BulletStatus *status = GameCreate<BulletStatus>(ext);
	BulletStatus* status = new BulletStatus(ext);
	globalScripts.emplace_back(status);

	// Other Component
	AttachFire<BulletClass, BulletExt>* attachFire = new AttachFire<BulletClass, BulletExt>(ext);
	globalScripts.emplace_back(attachFire);

	BulletTrail* trail = new BulletTrail(ext);
	globalScripts.emplace_back(trail);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
