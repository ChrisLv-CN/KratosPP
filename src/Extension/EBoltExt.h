#pragma once

#include <EBolt.h>

#include "GOExtension.h"

class EBoltExt : public GOExtension<EBolt, EBoltExt>
{
public:
	static constexpr DWORD Canary = 0x2C2C2C2C;

	/// @brief 创建Component实例，并加入到GameObject中.
	/// 创建时需要使用new，不能使用GameCreate.
	/// @param globalScripts 待附加的脚本列表
	/// @param ext ExtData
	static void AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext);

	static EBoltExt::ExtContainer ExtMap;
};
