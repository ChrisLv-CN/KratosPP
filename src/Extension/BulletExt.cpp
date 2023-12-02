#include "BulletExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/AttachFire.h>
#include <Ext/BulletStatus.h>
#include <Ext/BulletTrail.h>

void BulletExt::AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext)
{
	// Base Component
	globalScripts.push_back(BulletStatus::ScriptName);
	globalScripts.push_back(BulletTrail::ScriptName);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
std::vector<BulletClass*> BulletExt::TargetAircraftBullets{};
