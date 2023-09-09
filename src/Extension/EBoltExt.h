#pragma once

#include "GOExtension.h"

#include <EBolt.h>

class EBoltExt : public GOExtension<EBolt, EBoltExt>
{
public:
	static constexpr DWORD Canary = 0x2C2C2C2C;
	static constexpr size_t ExtPointerOffset = 0x18;

	static void AddGlobalScripts(std::list<Component *> *globalScripts, ExtData *ext);

	static EBoltExt::ExtContainer ExtMap;
};
