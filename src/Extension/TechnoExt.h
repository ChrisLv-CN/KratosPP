#pragma once

#include <Extension/GOExtension.h>
#include <Extension/TechnoTypeExt.h>

class TechnoExt;

class TechnoExt : public GOExtension<TechnoClass, TechnoExt>
{
public:
	static constexpr DWORD Canary = 0x55555555;
	static constexpr size_t ExtPointerOffset = 0x34C;

	static void AddGlobalScripts(std::list<ScriptComponent*>* globalScripts, TechnoClass* OwnerObject);

	static ExtContainer ExtMap;
};
