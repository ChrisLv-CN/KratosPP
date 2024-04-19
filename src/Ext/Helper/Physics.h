#pragma once
#include <string>
#include <map>
#include <vector>

#include <GeneralDefinitions.h>
#include <CellClass.h>
#include <MapClass.h>
#include <BuildingClass.h>
#include <InfantryClass.h>

#include <Common/INI/INIConfig.h>

#include "Status.h"

enum class PassError : int
{
	NONE = 0,
	PASS = 1, // 可通行
	UNDERGROUND = 2, // 潜地
	HITWALL = 3, // 不可通行
	HITBUILDING = 4, // 撞建筑
	DOWNBRIDGE = 5, // 从上方撞桥
	UPBRIDEG = 6 // 从下方撞桥
};

/**
 *@brief 是否撞击建筑物
 *
 * @param pBuilding 待检查的建筑物
 * @param targetZ 撞击位置的Z轴高度
 * @param blade 贯穿天地的剑气，必中
 * @param zOffset 高度偏移值
 * @return true 撞到了
 * @return false 没撞到
 */
bool CanHit(BuildingClass* pBuilding, int targetZ, bool blade = false, int zOffset = 0);

/**
 *@brief 前方格子是否可以通行，有无障碍物
 *
 * @param sourcePos 当前位置
 * @param nextPos 下一帧的位置
 * @param passBuilding 能穿透建筑
 * @param nextCellPos 目标位置的格子
 * @param onBridge 目标位置是桥
 * @return PassError
 */
PassError CanMoveTo(CoordStruct sourcePos, CoordStruct nextPos, bool passBuilding, CoordStruct& nextCellPos, bool& onBridge);

/**
 * @brief 检查脚下是否可以通行
 *
 * @param pTechno 检查的单位
 * @param targetPos 摔的位置
 * @param pCell 脚下的格子
 * @param isWater 脚下的格子是否是水面
 * @return true 可以通行
 * @return false 不可，应该摔死
 */
bool CanPassUnder(TechnoClass* pTechno, CoordStruct& targetPos, CellClass*& pCell, bool& isWater);

enum FallingError : int
{
	FLY = -1, // 没想到吧，哥会飞
	UNCHANGED = 0, // 停在地上
	FALLING = 1, // 掉落在地上
	SINKING = 2, // 沉入水中
	BOMB = 3 // 摔死
};

/**
 *@brief 强制摔在地上，如果已经在地上了则尝试散开，如果在空中则摔落在地
 *
 * @param pTechno 要摔的单位
 * @param targetPos 摔的位置
 * @param fallingDestroyHeight 强制摔死的高度
 * @param hasParachute 有降落伞
 * @param isWater 下方是否是水
 * @param canPass 可以安全进入
 * @return FallingError
 */
FallingError Falling(TechnoClass* pTechno, CoordStruct targetPos, int fallingDestroyHeight, bool hasParachute, bool isWater, bool& canPass);

/**
 *@brief 判断脚下能否着陆，然后往下摔
 *
 * @param pTechno 
 * @param fallingDestroyHeight 
 * @param hasParachute 
 * @return true 
 * @return false 
 */
FallingError FallingDown(TechnoClass* pTechno, int fallingDestroyHeight, bool hasParachute);

/**
 *@brief 除了会飞的单位之外，其他的单位往下坠落
 *
 * @param pTechno 要摔的单位
 * @param fallingDestroyHeight 强制摔死的高度
 * @param hasParachute 有降落伞
 * @return true 安全下落
 * @return false BOOM!
 */
FallingError FallingExceptAircraft(TechnoClass* pTechno, int fallingDestroyHeight, bool hasParachute);
