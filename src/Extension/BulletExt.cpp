#include "BulletExt.h"

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/BulletType/BulletTrail.h>
#include <Ext/ObjectType/AttachEffect.h>

void BulletExt::AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext)
{
	// Base Component
	globalScripts.push_back(BulletStatus::ScriptName);
	globalScripts.push_back(AttachEffect::ScriptName);
	globalScripts.push_back(BulletTrail::ScriptName);
}

BulletExt::ExtContainer BulletExt::ExtMap{};
std::vector<BulletClass*> BulletExt::TargetHasDecoyBullets{};
