#include "TechnoExt.h"

#include <Ext/AttachEffect.h>
#include <Ext/TechnoStatus.h>
#include <Ext/LaserTrail.h>

void TechnoExt::AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext)
{
	// Base Component
	// can't use GameCreate because can't use GameDelete to release memory.
	// TechnoStatus *status = GameCreate<TechnoStatus>(ext);
	TechnoStatus* status = new TechnoStatus(ext);
#ifdef DEBUG
	char status_this[1024];
	sprintf_s(status_this, "%p", status);
	std::string status_Id = { status_this };
	Debug::Log("Component TechnoStatus %s created \n", status_Id.c_str());
#endif // DEBUG
	globalScripts.emplace_back(status);

	// Other Components
	LaserTrail* laserTrail = new LaserTrail(ext);
#ifdef DEBUG
	char laser_this[1024];
	sprintf_s(laser_this, "%p", laserTrail);
	std::string laser_Id = { laser_this };
	Debug::Log("Component LaserTrail %s created \n", laser_Id.c_str());
#endif // DEBUG
	globalScripts.emplace_back(laserTrail);

	AttachEffect* attachEffect = new AttachEffect(ext);
#ifdef DEBUG
	char ae_this[1024];
	sprintf_s(ae_this, "%p", attachEffect);
	std::string ae_Id = { ae_this };
	Debug::Log("Component AttachEffect %s created \n", ae_Id.c_str());
#endif // DEBUG
	globalScripts.emplace_back(attachEffect);
}

TechnoExt::ExtContainer TechnoExt::ExtMap{};
