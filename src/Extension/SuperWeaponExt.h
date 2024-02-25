#pragma once

#include <SuperClass.h>

#include "GOExtension.h"

class SuperWeaponExt : public GOExtension<SuperClass, SuperWeaponExt>
{
public:
	static constexpr DWORD Canary = 0x55555555;
	// static constexpr size_t ExtPointerOffset = 0x34C;

	/// @brief 创建Component实例，并加入到GameObject中.
	/// 创建时需要使用new，不能使用GameCreate.
	/// @param globalScripts 待附加的脚本列表
	/// @param ext ExtData
	static void AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext);

	static SuperWeaponExt::ExtContainer ExtMap;
};
