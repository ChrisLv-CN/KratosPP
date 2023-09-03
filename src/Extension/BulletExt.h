#pragma once

#include "GOExtension.h"

#include <BulletClass.h>

class BulletExt : public GOExtension<BulletClass, BulletExt>
{
public:
	static constexpr DWORD Canary = 0x2A2A2A2A;
	static constexpr size_t ExtPointerOffset = 0x18;

	static void AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext);

	static BulletExt::ExtContainer ExtMap;
};
