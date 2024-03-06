#include "HealthText.h"

#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/Common/PrintTextManager.h>

HealthTextData HealthText::GetHealthTextData()
{
	if (!_healthTextData.IsRead)
	{
		HealthTextControlData* controlData = &TechnoExt::HealthTextControlData;
		if (!controlData->IsRead)
		{
			controlData->Read();
		}
		// 读取全局设置
		if (IsBuilding())
		{
			_healthTextData = controlData->Building;
		}
		else if (IsInfantry())
		{
			_healthTextData = controlData->Infantry;
		}
		else if (IsUnit())
		{
			_healthTextData = controlData->Unit;
		}
		else if (IsAircraft())
		{
			_healthTextData = controlData->Aircraft;
		}
		// 读取个体设置
		INIBufferReader* reader = INI::GetSection(INI::Rules, pTechno->GetTechnoType()->ID);
		_healthTextData.Read(reader);
	}
	return _healthTextData;
}

void HealthText::PrintHealthText(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding)
{
	bool isSelected = pTechno->IsSelected;
	// 根据血量状态获取设置
	HealthTextEntity data = GetHealthTextData().Green;
	HealthState healthState = pTechno->GetHealthStatus();
	switch (healthState)
	{
	case HealthState::Yellow:
		data = GetHealthTextData().Yellow;
		break;
	case HealthState::Red:
		data = GetHealthTextData().Red;
		break;
	}
	if (data.Hidden || (!pTechno->Owner->IsCurrentPlayer() && !data.ShowEnemy) || (!isSelected && !data.ShowHover))
	{
		return;
	}
	// 调整锚点
	Point2D pos = *pPos;
	int xOffset = data.Offset.X; // 锚点向右的偏移值
	int yOffset = data.Offset.Y; // 锚点向下的偏移值
	int barWidth = barLength * 2; // 血条显示的个数，单位是半条，建筑是满条

	// Point2D fountSize = data.FontSize; // 使用shp则按照shp图案大小来偏移锚点
	HealthTextStyle style = isSelected ? data.Style : data.HoverStyle; ; // 数值的格式
	if (isBuilding)
	{
		// 算出建筑血条最左边格子的偏移
		CoordStruct dimension;
		pTechno->GetTechnoType()->Dimension2(&dimension);
		CoordStruct dimension2{ -dimension.X / 2, dimension.Y / 2, dimension.Z };
		Point2D pos2;
		TacticalClass::Instance->CoordsToScreen(&pos2, &dimension2);
		// 修正锚点
		pos += pos2;
		pos.X = pos.X - 2 + xOffset;
		pos.Y = pos.Y - 2 + yOffset;
		barWidth = barLength * 4 + 6; // 建筑是满条，每个块是10像素宽，每个4像素绘制一个，头边距2，尾边距4
	}
	else
	{
		yOffset += pTechno->GetTechnoType()->PixelSelectionBracketDelta;
		pos.X += -barLength + 3 + xOffset;
		pos.Y += -28 + yOffset;
		if (barLength == 8)
		{
			// 步兵血条 length = 8
			pos.X += 1;
		}
		else
		{
			// 载具血条 length = 17
			pos.Y += -1;
		}
	}
	// 获得血量数据
	std::wstring text{ L"" };
	int health = pTechno->Health;
	std::wstring s = isBuilding ? L"|" : L"/";
	switch (style)
	{
	case HealthTextStyle::FULL:
	{
		int strength = pTechno->GetTechnoType()->Strength;
		text = std::format(L"{0}{1}{2}", std::to_wstring(health), s, std::to_wstring(strength));
		break;
	}
	case HealthTextStyle::PERCENT:
	{
		int per = (int)(pTechno->GetHealthPercentage() * 100);
		text = std::format(L"{0}%", std::to_wstring(per));
		break;
	}
	default:
		text = std::to_wstring(health);
		break;
	}
	if (!text.empty())
	{
		// 修正锚点
		if (data.UseSHP)
		{
			// 使用Shp显示数字，SHP锚点在图案中心
			// 重新调整锚点位置，向上抬起一个半格字的高度
			pos.Y = pos.Y - data.ImageSize.Y / 2;

			// 按对齐方式再次调整锚点
			if (data.Align != PrintTextAlign::LEFT)
			{
				int x = data.ImageSize.X % 2 == 0 ? data.ImageSize.X : data.ImageSize.X + 1;
				int textWidth = text.size() * x;
				OffsetPosAlign(pos, textWidth, barWidth, data.Align, isBuilding, true);
			}
			else
			{
				if (isBuilding)
				{
					pos.X += data.ImageSize.X; // 右移一个字宽，美观
				}
			}
		}
		else
		{
			// 使用文字显示数字，文字的锚点在左上角
			// 重新调整锚点位置，向上抬起一个半格字的高度
			pos.Y = pos.Y - PrintTextManager::GetFontSize().Y + 5; // 字是20格高，上4中9下7

			// 按对齐方式再次调整锚点
			if (data.Align != PrintTextAlign::LEFT)
			{
				RectangleStruct textRect = Drawing::GetTextDimensions(text.c_str(), Point2D::Empty, 0, 2, 0);
				int textWidth = textRect.Width;
				OffsetPosAlign(pos, textWidth, barWidth, data.Align, isBuilding, false);
			}
			else
			{
				if (isBuilding)
				{
					pos.X += PrintTextManager::GetFontSize().X; // 右移一个字宽，美观
				}
				else
				{
					pos.X -= PrintTextManager::GetFontSize().X / 2; // 左移半个字宽，美观
				}
			}
		}
		PrintTextManager::Print(text, pTechno->Owner->LaserColor, data, pos, pBound, DSurface::Temp, isBuilding);
	}
}

void HealthText::OffsetPosAlign(Point2D& pos, int textWidth, int barWidth, PrintTextAlign align, bool isBuilding, bool useSHP)
{
	int offset = barWidth - textWidth;
	switch (align)
	{
	case PrintTextAlign::CENTER:
		pos.X += offset / 2;
		if (isBuilding)
		{
			pos.Y -= offset / 4;
		}
		break;
	case PrintTextAlign::RIGHT:
		pos.X += offset;
		if (!useSHP)
		{
			pos.X += PrintTextManager::GetFontSize().X / 2; // 右移半个字宽，补偿Margin
		}
		if (isBuilding)
		{
			pos.Y -= offset / 2;
		}
		break;
	}
}

void HealthText::Awake()
{
	if (GetHealthTextData().Hidden)
	{
		Disable();
	}
}

void HealthText::DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding)
{
	if (!GetHealthTextData().Hidden)
	{
		PrintHealthText(barLength, pPos, pBound, isBuilding);
	}
}


