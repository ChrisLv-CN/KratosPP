#pragma once

#include <BulletClass.h>

#include "GOExtension.h"

class BulletExt : public GOExtension<BulletClass, BulletExt>
{
public:
	static constexpr DWORD Canary = 0x2A2A2A2A;
	// static constexpr size_t ExtPointerOffset = 0x18;

	/**
	 * @brief 创建Component实例，并加入到GameObject中.
	 * 创建时需要使用new，不能使用GameCreate.
	 * @param globalScripts 待附加的脚本列表
	 * @param ext ExtData
	 */
	static void AddGlobalScripts(std::list<std::string>& globalScripts, ExtData* ext);

	static BulletExt::ExtContainer ExtMap;

	/**
	 *@brief 目标为空军的抛射体清单
	 *
	 */
	static std::vector<BulletClass*> TargetHasDecoyBullets;
};
