#pragma once

#include "GOExtension.h"

#include <SuperClass.h>

class SuperWeaponExt : public GOExtension<SuperClass, SuperWeaponExt>
{
public:
	static constexpr DWORD Canary = 0x55555555;
	static constexpr size_t ExtPointerOffset = 0x34C;

	static void AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext);

	static SuperWeaponExt::ExtContainer ExtMap;
};
