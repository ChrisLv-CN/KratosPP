#pragma once

#include "GOExtension.h"

#include <AnimClass.h>

class AnimExt : public GOExtension<AnimClass, AnimExt>
{
public:
	static constexpr DWORD Canary = 0xAAAAAAAA;
	static constexpr size_t ExtPointerOffset = 0xD0;

	static void AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext);

	static AnimExt::ExtContainer ExtMap;
};
