#include "BulletExt.h"

#include <Ext/BulletStatus.h>

void BulletExt::AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext)
{
	BulletStatus *status = GameCreate<BulletStatus>(ext);
	globalScripts->push_back(status);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
