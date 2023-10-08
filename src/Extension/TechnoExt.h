#pragma once

#include <TechnoClass.h>

#include "GOExtension.h"
#include <Ext/TechnoType/HealthTextData.h>


class TechnoExt : public GOExtension<TechnoClass, TechnoExt>
{
public:
	static constexpr DWORD Canary = 0x55555555;
	static constexpr size_t ExtPointerOffset = 0x34C;

	/**
	 * @brief 创建Component实例，并加入到GameObject中.
	 * 创建时需要使用new，不能使用GameCreate.
	 * @param globalScripts 待附加的脚本列表
	 * @param ext ExtData
	 */
	static void AddGlobalScripts(std::list<Component*>& globalScripts, ExtData* ext);

	static TechnoExt::ExtContainer ExtMap;

	/**
	 *@brief 储存可以作为基地建造节点的单位.
	 * key = 单位, value = 做友军的基地建造节点
	 */
	static std::map<TechnoClass*, bool> BaseUnitArray;

	/**
	 *@brief 储存可以作为基地建造节点的替身单位.
	 * key = 单位, value = 做友军的基地建造节点
	 */
	static std::map<TechnoClass*, bool> BaseStandArray;

	/**
	 *@brief 血量数字的全局设置
	 *
	 */
	static HealthTextControlData HealthTextControlData;
};
