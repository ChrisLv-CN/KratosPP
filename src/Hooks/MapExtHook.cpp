#include <exception>
#include <Windows.h>
#include <set>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <BuildingClass.h>
#include <DisplayClass.h>

#include <Ext/Helper.h>
#include <Ext/TechnoType/BuildingRangeData.h>




#pragma region Show Building Range
DEFINE_HOOK(0x6D5116, TacticalClass_Draw_Placement_Recheck, 0x5)
{
	BuildingTypeClass* pBuildingType = DisplayClass::Display_PendingObject.get();
	if (pBuildingType->What_Am_I() == AbstractType::BuildingType)
	{
		BuildingRangeData* data = INI::GetConfig<BuildingRangeData>(INI::Rules, pBuildingType->ID)->Data;
		if (data->Mode != BuildingRangeMode::NONE)
		{
			// 获取建筑的四个角
			int width = pBuildingType->GetFoundationWidth();
			int height = pBuildingType->GetFoundationHeight(false);
			CellStruct zoneCell = DisplayClass::Display_ZoneCell.get();
			CellStruct zoneOffset = DisplayClass::Display_ZoneOffset.get();
			CellStruct center = zoneCell + zoneOffset;
			int cellX = center.X;
			int cellY = center.Y;
			int adjust = pBuildingType->Adjacent + 1;
			// 北
			CellStruct nCell{ static_cast<short>(cellX - adjust), static_cast<short>(cellY - adjust) };
			// 东
			CellStruct eCell{ static_cast<short>(cellX + adjust + width - 1), static_cast<short>(cellY - adjust) };
			// 南
			CellStruct sCell{ static_cast<short>(cellX + adjust + width - 1), static_cast<short>(cellY + adjust + height - 1) };
			// 西
			CellStruct wCell{ static_cast<short>(cellX - adjust), static_cast<short>(cellY + adjust + height - 1) };
			// 可视范围
			DSurface* pSurface = DSurface::Temp;
			RectangleStruct rect = pSurface->GetRect();
			rect.Height -= 34;
			int color = Drawing::RGB_To_Int(data->Color);
			// 开始渲染
			switch (data->Mode)
			{
			case BuildingRangeMode::LINE:
			{
				// 北
				CoordStruct nPos = CellClass::Cell2Coord(nCell);
				if (CellClass* pNCell = MapClass::Instance->TryGetCellAt(nCell))
				{
					nPos = pNCell->GetCenterCoords();
				}
				nPos.X -= 128;
				nPos.Y -= 128;
				Point2D n = ToClientPos(nPos);
				// 东
				CoordStruct ePos = CellClass::Cell2Coord(eCell);
				if (CellClass* pECell = MapClass::Instance->TryGetCellAt(eCell))
				{
					ePos = pECell->GetCenterCoords();
				}
				ePos.X += 128;
				ePos.Y -= 128;
				Point2D e = ToClientPos(ePos);
				// 南
				CoordStruct sPos = CellClass::Cell2Coord(sCell);
				if (CellClass* pSCell = MapClass::Instance->TryGetCellAt(sCell))
				{
					sPos = pSCell->GetCenterCoords();
				}
				sPos.X += 128;
				sPos.Y += 128;
				Point2D s = ToClientPos(sPos);
				// 西
				CoordStruct wPos = CellClass::Cell2Coord(wCell);
				if (CellClass* pWCell = MapClass::Instance->TryGetCellAt(wCell))
				{
					wPos = pWCell->GetCenterCoords();
				}
				wPos.X -= 128;
				wPos.Y += 128;
				Point2D w = ToClientPos(wPos);
				if (data->Dashed)
				{
					// 处理四角越界并绘制
					DrawDashedLine(pSurface, n, e, color, rect);
					DrawDashedLine(pSurface, e, s, color, rect);
					DrawDashedLine(pSurface, s, w, color, rect);
					DrawDashedLine(pSurface, w, n, color, rect);
				}
				else
				{
					// 处理四角越界并绘制
					DrawLine(pSurface, n, e, color, rect);
					DrawLine(pSurface, e, s, color, rect);
					DrawLine(pSurface, s, w, color, rect);
					DrawLine(pSurface, w, n, color, rect);
				}
				break;
			}
			default:
			{
				// 有效范围
				int minX = nCell.X;
				int minY = nCell.Y;
				int maxX = eCell.X;
				int maxY = wCell.Y;
				// 可视范围
				int minVX = rect.X;
				int maxVX = rect.X + rect.Width;
				int minVY = rect.Y;
				int maxVY = rect.Y + rect.Height;
				// Logger.Log($"{Game.CurrentFrame} 可视范围 [{minVX} - {maxVX}], [{minVY} - {maxVY}]");
				// 获取所有的Cell，捡出在视野范围内的Cell
				std::set<CellClass*> cells{};
				for (int y = minY; y <= maxY; y++)
				{
					for (int x = minX; x <= maxX; x++)
					{
						CellStruct cellPos{ static_cast<short>(x), static_cast<short>(y) };
						CoordStruct location = CellClass::Cell2Coord(cellPos);
						Point2D point = ToClientPos(location);
						// 在可视范围内
						if (point.X >= minVX && point.X <= maxVX && point.Y >= minVY && point.Y <= maxVY)
						{
							if (CellClass* pCell = MapClass::Instance->TryGetCellAt(cellPos))
							{
								cells.insert(pCell);
							}
						}
					}
				}
				// 遍历每一格，绘制相应的图像
				switch (data->Mode)
				{
				case BuildingRangeMode::CELL:
				{
					for (CellClass* pCell : cells)
					{
						if (pCell->SlopeIndex == 0)
						{
							CoordStruct cellPos = pCell->GetCoordsWithBridge();
							CoordStruct pE = cellPos + CoordStruct{ 128, -128, 0 };
							Point2D e = ToClientPos(pE);
							CoordStruct pW = cellPos + CoordStruct{ -128, 128, 0 };
							Point2D w = ToClientPos(pW);
							CoordStruct pN = cellPos + CoordStruct{ -128, -128, 0 };
							Point2D n = ToClientPos(pN);
							CoordStruct pS = cellPos + CoordStruct{ 128, 128, 0 };
							Point2D s = ToClientPos(pS);
							if (data->Dashed)
							{
								// 处理四角越界并绘制
								DrawDashedLine(pSurface, n, e, color, rect);
								DrawDashedLine(pSurface, e, s, color, rect);
								DrawDashedLine(pSurface, s, w, color, rect);
								DrawDashedLine(pSurface, w, n, color, rect);
							}
							else
							{
								// 处理四角越界并绘制
								DrawLine(pSurface, n, e, color, rect);
								DrawLine(pSurface, e, s, color, rect);
								DrawLine(pSurface, s, w, color, rect);
								DrawLine(pSurface, w, n, color, rect);
							}
						}
					}
					break;
				}
				case BuildingRangeMode::SHP:
				{
					if (IsNotNone(data->SHPFileName))
					{
						if (SHPStruct* pCustomSHP = FileSystem::LoadSHPFile(data->SHPFileName.c_str()))
						{
							ConvertClass* pPalette = FileSystem::PALETTE_PAL;
							for (CellClass* pCell : cells)
							{
								// WWSB
								CellStruct cell = pCell->MapCoords;
								CoordStruct newPos{ ((((cell.X << 8) + 128) / 256) << 8), ((((cell.Y << 8) + 128) / 256) << 8), 0 };
								Point2D position = CoordsToScreen(newPos);
								position -= TacticalClass::Instance->TacticalPos;
								int zAdjust = 15 * pCell->Level;
								position.Y += -1 - zAdjust;
								int frame = pCell->SlopeIndex + 2;
								// 显示对应的帧
								pSurface->DrawSHP(pPalette, pCustomSHP, data->ZeroFrameIndex + frame, &position);
							}
						}
					}
					break;
				}
				}
				break;
			}
			}

			DisplayClass::Display_PassedProximityCheck.get() = DisplayClass::Instance->Passes_Proximity_Check();
		}
	}
	return 0;
}


DEFINE_HOOK(0x4A904E, DisplayClass_Passes_Proximity_Check_MobileMCV, 0x5)
{
	return 0;
}
#pragma endregion
