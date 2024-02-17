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

static bool InRect(Point2D point, RectangleStruct bound)
{
	return point.X >= bound.X && point.X <= bound.X + bound.Width && point.Y >= bound.Y && point.Y <= bound.Y + bound.Height;
}

static bool InFog(CoordStruct location)
{
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		if (pCell->Flags & CellFlags::Revealed)
		{
			return false;
		}
	}
	return true;
}

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
static bool DrawDashedLine(Surface* pSurface, Point2D point1, Point2D point2, int dwColor, RectangleStruct bound, bool blink = false)
{
	if (bound.IsEmpty())
	{
		bound = pSurface->GetRect();
	}
	Surface::ClipLine(point1, point2, bound);
	// point in rect then draw
	if (InRect(point1, bound) && InRect(point2, bound))
	{
		int offset = 0;
		if (blink)
		{
			offset = 7 * Unsorted::CurrentFrame % 16;
		}
		return pSurface->DrawDashedLine(&point1, &point2, dwColor, offset);
	}
	return false;
}

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
static bool DrawDashedLine(Surface* pSurface, Point2D point1, Point2D point2, ColorStruct color, RectangleStruct bound, bool blink = false)
{
	return DrawDashedLine(pSurface, point1, point2, Drawing::RGB_To_Int(color), bound, blink);
}

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
static bool DrawLine(Surface* pSurface, Point2D point1, Point2D point2, int dwColor, RectangleStruct bound)
{
	if (bound.IsEmpty())
	{
		bound = pSurface->GetRect();
	}
	Surface::ClipLine(point1, point2, bound);
	// point in rect then draw
	if (InRect(point1, bound) && InRect(point2, bound))
	{
		return pSurface->DrawLine(&point1, &point2, dwColor);
	}
	return false;
}

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
static bool DrawLine(Surface* pSurface, Point2D point1, Point2D point2, ColorStruct color, RectangleStruct bound)
{
	return DrawLine(pSurface, point1, point2, Drawing::RGB_To_Int(color), bound);
}

static void DrawCrosshair(Surface* pSurface, CoordStruct sourcePos, int length, ColorStruct color, RectangleStruct bounds = {}, bool dashed = true, bool blink = false)
{
	CoordStruct p1 = sourcePos + CoordStruct{ length, 0, 0 };
	CoordStruct p2 = sourcePos + CoordStruct{ -length, 0, 0 };
	CoordStruct p3 = sourcePos + CoordStruct{ 0, length, 0 };
	CoordStruct p4 = sourcePos + CoordStruct{ 0, -length, 0 };
	if (dashed)
	{
		DrawDashedLine(pSurface, ToClientPos(p1), ToClientPos(p2), color, bounds, blink);
		DrawDashedLine(pSurface, ToClientPos(p3), ToClientPos(p4), color, bounds, blink);
	}
	else
	{
		DrawLine(pSurface, ToClientPos(p1), ToClientPos(p2), color, bounds);
		DrawLine(pSurface, ToClientPos(p3), ToClientPos(p4), color, bounds);
	}
}

static void DrawCell(Surface* pSurface, CoordStruct sourcePos, ColorStruct color, RectangleStruct bounds = {}, bool dashed = true, bool blink = false, int length = 128)
{
	CoordStruct p1 = sourcePos + CoordStruct{ length, length, 0 };
	CoordStruct p2 = sourcePos + CoordStruct{ -length, length, 0 };
	CoordStruct p3 = sourcePos + CoordStruct{ -length, -length, 0 };
	CoordStruct p4 = sourcePos + CoordStruct{ length, -length, 0 };
	if (dashed)
	{
		DrawDashedLine(pSurface, ToClientPos(p1), ToClientPos(p2), color, bounds);
		DrawDashedLine(pSurface, ToClientPos(p2), ToClientPos(p3), color, bounds);
		DrawDashedLine(pSurface, ToClientPos(p3), ToClientPos(p4), color, bounds);
		DrawDashedLine(pSurface, ToClientPos(p4), ToClientPos(p1), color, bounds);
	}
	else
	{
		DrawLine(pSurface, ToClientPos(p1), ToClientPos(p2), color, bounds);
		DrawLine(pSurface, ToClientPos(p2), ToClientPos(p3), color, bounds);
		DrawLine(pSurface, ToClientPos(p3), ToClientPos(p4), color, bounds);
		DrawLine(pSurface, ToClientPos(p4), ToClientPos(p1), color, bounds);
	}
}
