#pragma once

#include "GOExtension.h"

#include <TechnoClass.h>
#include <TechnoTypeClass.h>

class TechnoExt;

class TechnoExt : public GOExtension<TechnoClass, TechnoExt>
{
public:
	static constexpr DWORD Canary = 0x55555555;
	static constexpr size_t ExtPointerOffset = 0x34C;

	static void AddGlobalScripts(std::list<Component*>* globalScripts, ExtData* ext);

	static TechnoExt::ExtContainer ExtMap;
};
