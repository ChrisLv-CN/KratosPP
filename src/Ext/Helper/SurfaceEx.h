#pragma once
#include <string>
#include <map>
#include <vector>

#include <GeneralDefinitions.h>
#include <CellClass.h>
#include <MapClass.h>

#include <Common/INI/INIConfig.h>

#include "CastEx.h"

static std::map<std::string, Layer> LayerStrings
{
	{ "None", Layer::None },
	{ "Underground", Layer::Underground },
	{ "Surface", Layer::Surface },
	{ "Ground", Layer::Ground },
	{ "Air", Layer::Air },
	{ "Top", Layer::Top }
};

template <>
inline bool Parser<Layer>::TryParse(const char* pValue, Layer* outValue)
{
	std::string key = pValue;
	auto it = LayerStrings.find(key);
	if (it != LayerStrings.end())
	{
		*outValue = it->second;
		return true;
	}
	return false;
}

bool InRect(Point2D point, RectangleStruct bound);

bool InFog(CoordStruct location);

/**
 * @brief 处理四角越界并绘制虚线
 *
 * @param pSurface 面板
 * @param point1 起始点
 * @param point2 终止点
 * @param dwColor 颜色
 * @param bound 区域
 * @param blink 闪烁
 * @return true 成功绘制
 * @return false 绘制失败
 */
bool DrawDashedLine(DSurface* pSurface,
	Point2D point1, Point2D point2, int dwColor, RectangleStruct bound,
	bool blink = false);

/**
 * @brief 处理四角越界并绘制虚线
 *
 * @param pSurface 面板
 * @param point1 起始点
 * @param point2 终止点
 * @param color 颜色
 * @param bound 区域
 * @param blink 闪烁
 * @return true 成功绘制
 * @return false 绘制失败
 */
bool DrawDashedLine(DSurface* pSurface,
	Point2D point1, Point2D point2, ColorStruct color, RectangleStruct bound,
	bool blink = false);

/**
 * @brief 处理四角越界并绘制线条
 *
 * @param pSurface 面板
 * @param point1 起始点
 * @param point2 终止点
 * @param dwColor 颜色
 * @param bound 区域
 * @return true 成功绘制
 * @return false 绘制失败
 */
bool DrawLine(DSurface* pSurface,
	Point2D point1, Point2D point2, int dwColor, RectangleStruct bound);

/**
 * @brief 处理四角越界并绘制线条
 *
 * @param pSurface 面板
 * @param point1 起始点
 * @param point2 终止点
 * @param color 颜色
 * @param bound 区域
 * @return true 成功绘制
 * @return false 绘制失败
 */
bool DrawLine(DSurface* pSurface,
	Point2D point1, Point2D point2, ColorStruct color, RectangleStruct bound);

void DrawCrosshair(DSurface* pSurface,
	CoordStruct sourcePos, int length, ColorStruct color, RectangleStruct bounds = {},
	bool dashed = true, bool blink = false);

void DrawCell(DSurface* pSurface,
	CoordStruct sourcePos, ColorStruct color, RectangleStruct bounds = {},
	bool dashed = true, bool blink = false, int length = 128);

/**
 *@brief 绘制一个类激光照射的3x3范围的十字点
 *
 * @param pSurface
 * @param location
 * @param color
 * @param bound
 */
void DrawTargetLaserPoint(DSurface* pSurface,
	CoordStruct location, ColorStruct color, RectangleStruct bound);

/**
 *@brief 绘制激光照射的十字点
 *
 * @param pSurface
 * @param targetPos
 * @param dwColor
 * @param bound
 */
void DrawTargetLaserPoint(DSurface* pSurface,
	Point2D targetPos, int dwColor, RectangleStruct bound);

/**
 *@brief 绘制激光照射
 *
 * @param pSurface
 * @param start
 * @param end
 * @param color
 * @param bound
 */
void DrawTargetLaser(DSurface* pSurface,
	CoordStruct start, CoordStruct end, ColorStruct color, RectangleStruct bound,
	bool drawPoint = true);

