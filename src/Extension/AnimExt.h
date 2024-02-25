#pragma once

#include <AnimClass.h>

#include "GOExtension.h"

class AnimExt : public GOExtension<AnimClass, AnimExt>
{
public:
	static constexpr DWORD Canary = 0xAAAAAAAA;
	// static constexpr size_t ExtPointerOffset = 0xD0;

	/// @brief 创建Component实例，并加入到GameObject中.
	/// 创建时需要使用new，不能使用GameCreate.
	/// @param globalScripts 待附加的脚本列表
	/// @param ext ExtData
	static void AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext);

	static AnimExt::ExtContainer ExtMap;
};
