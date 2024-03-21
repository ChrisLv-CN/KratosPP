#pragma once

#include <string>
#include <vector>

#include <Common/EventSystems/EventSystem.h>

#include "PrintTextData.h"


class PrintTextManager
{
public:
	static Point2D GetFontSize();

	static Point2D GetFontSize(std::wstring font);

	/**
	 *@brief 在指定位置打印一个文字
	 *
	 * @param text 内容
	 * @param houseColor 颜色
	 * @param data 格式
	 * @param pos 位置
	 * @param pBound 范围
	 * @param pSurface 渲染器
	 * @param isBuilding 是否是建筑
	 */
	static void Print(std::wstring text, ColorStruct houseColor, PrintTextData data, Point2D pos, RectangleStruct* pBound, DSurface* pSurface, bool isBuilding = false);

	/**
	 *@brief 在指定位置打印一个数字/进度
	 *
	 * @param number 内容
	 * @param houseColor 颜色
	 * @param data 格式
	 * @param pos 位置
	 * @param pBound 范围
	 * @param pSurface 渲染器
	 * @param isBuilding 是否是建筑
	 */
	static void Print(int number, ColorStruct houseColor, PrintTextData data, Point2D pos, RectangleStruct* pBound, DSurface* pSurface, bool isBuilding = false);

	static void PrintText(std::string text, ColorStruct houseColor, Point2D pos, PrintTextData data);
	static void PrintText(std::string text, ColorStruct houseColor, CoordStruct location, PrintTextData data);

	static void PrintText(std::string text, ColorStruct color, Point2D pos);
	static void PrintText(std::string text, ColorStruct color, CoordStruct location);

	static void PrintNumber(int number, ColorStruct houseColor, Point2D pos, PrintTextData data);
#pragma region Rolling Text
	static void Clear(EventSystem* sender, Event e, void* args);

	/**
	 *@brief 添加一条待打印的漂浮文字
	 *
	 * @param text 内容
	 * @param location 起始位置
	 * @param offset 位置偏移
	 * @param rollSpeed 滚动速度
	 * @param duration 持续时间
	 * @param data 格式
	 */
	static void AddRollingText(std::wstring text, CoordStruct location, Point2D offset, int rollSpeed, int duration, PrintTextData data);

	static void PrintRollingText(EventSystem* sender, Event e, void* args);
#pragma endregion
private:
	static Point2D _fontSize;

	static std::vector<RollingText> _rollingTexts;
};
