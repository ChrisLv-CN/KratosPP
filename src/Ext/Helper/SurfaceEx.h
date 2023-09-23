#pragma once

#include <CellClass.h>
#include <GeneralStructures.h>
#include <MapClass.h>

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
