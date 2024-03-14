#include <exception>
#include <Windows.h>
#include <set>

#include <TechnoClass.h>
#include <HouseClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>
#include <Extension/TechnoTypeExt.h>
#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/Common/CommonStatus.h>
#include <Ext/Common/PrintTextManager.h>
#include <Ext/TechnoType/TechnoStatus.h>

#include <Ext/TechnoType/BuildingRangeData.h>

DEFINE_HOOK(0x489280, MapClass_DamageArea, 0x6)
{
	GET(CoordStruct*, pLocation, ECX);
	GET(int, damage, EDX);
	GET_STACK(ObjectClass*, pAttacker, 0x4);
	GET_STACK(WarheadTypeClass*, pWH, 0x8);
	// GET_STACK(bool, affectsTiberium, 0xC);
	GET_STACK(HouseClass*, pAttackingHouse, 0x10);
	if (pWH)
	{
		// 抛射体爆炸OnDetonate()后会调用该事件
		if (pAttackingHouse)
		{
			if (WarheadTypeExt::TypeData* warheadTypeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH))
			{
				int money = warheadTypeData->ShowMeTheMoney;
				if (money != 0)
				{
					pAttackingHouse->TransactMoney(warheadTypeData->ShowMeTheMoney);
					if (warheadTypeData->ShowMeTheMoneyDisplay)
					{
						DamageTextEntity textData;
						std::wstring text;
						if (money < 0)
						{
							text.append(L"-$").append(std::to_wstring(abs(money)));
							textData.Setup(true);
						}
						else
						{
							text.append(L"+$").append(std::to_wstring(money));
							textData.Setup(false);
						}
						CoordStruct location = *pLocation;
						PrintTextManager::AddRollingText(text, location, Point2D{ 0,0 }, textData.RollSpeed, textData.Duration, textData);
					}
				}
			}
		}
		// Find and Attach Effects.
		FindAndAttachEffect(*pLocation, damage, pWH, pAttacker, pAttackingHouse);
		// Find all stand, check distance and blown it up.
		FindAndDamageStandOrVUnit(*pLocation, damage, pWH, pAttacker, pAttackingHouse);
		// Those action won't effects Stand.
		TechnoClass* pTechno = nullptr;
		TechnoStatus* status = nullptr;
		if (pAttacker && CastToTechno(pAttacker, pTechno) && TryGetStatus<TechnoExt>(pTechno, status))
		{
			status->Teleport->Teleport(pLocation, pWH);
		}
	}
	return 0;
}

#pragma region Crate buff

/*
	generic crate-handler file
	currently used only to shim crates into TechnoExt
	since Techno fields are used by AttachEffect

	Graion Dilach, 2013-05-31
*/

//overrides for crate checks
//481D52 - pass
//481C86 - override with Money

DEFINE_HOOK(0x481D0E, CellClass_CrateBeingCollected_Firepower1, 0x6)
{
	GET(TechnoClass*, pTechno, EDI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CrateBuff.FirepowerMultiplier == 1.0)
		{
			return 0x481D52;
		}
		return 0x481C86;
	}
	return 0;
}

DEFINE_HOOK(0x481C6C, CellClass_CrateBeingCollected_Armor1, 0x6)
{
	GET(TechnoClass*, pTechno, EDI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CrateBuff.ArmorMultiplier == 1.0)
		{
			return 0x481D52;
		}
		return 0x481C86;
	}
	return 0;
}

DEFINE_HOOK(0x481CE1, CellClass_CrateBeingCollected_Speed1, 0x6)
{
	GET(TechnoClass*, pTechno, EDI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CrateBuff.SpeedMultiplier == 1.0)
		{
			return 0x481D52;
		}
		return 0x481C86;
	}
	return 0;
}

DEFINE_HOOK(0x481D3D, CellClass_CrateBeingCollected_Cloak1, 0x6)
{
	GET(TechnoClass*, pTechno, EDI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CanICloakByDefault() || status->CrateBuff.Cloakable)
		{
			return 0x481C86;
		}
		// cloaking forbidden for type
		if (!GetTypeData<TechnoTypeExt, TechnoTypeExt::TypeData>(pTechno->GetTechnoType())->AllowCloakable)
		{
			return 0x481C86;
		}
		return 0x481D52;
	}
	return 0;
}

//overrides on actual crate effect applications
DEFINE_HOOK(0x483226, CellClass_CrateBeingCollected_Firepower2, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	GET_STACK(double, pow_FirepowerMultiplier, 0x20);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CrateBuff.FirepowerMultiplier == 1.0)
		{
			status->CrateBuff.FirepowerMultiplier = pow_FirepowerMultiplier;
			status->RecalculateStatus();
			R->AL(pTechno->GetOwningHouse()->IsInPlayerControl);
			return 0x483258;
		}
	}
	return 0x483261;
}

DEFINE_HOOK(0x482E57, CellClass_CrateBeingCollected_Armor2, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	GET_STACK(double, pow_ArmorMultiplier, 0x20);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CrateBuff.ArmorMultiplier == 1.0)
		{
			status->CrateBuff.ArmorMultiplier = pow_ArmorMultiplier;
			status->RecalculateStatus();
			R->AL(pTechno->GetOwningHouse()->IsInPlayerControl);
			return 0x482E89;
		}
	}
	return 0x482E92;
}

DEFINE_HOOK(0x48303A, CellClass_CrateBeingCollected_Speed2, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	GET_STACK(double, pow_SpeedMultiplier, 0x20);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		if (status->CrateBuff.SpeedMultiplier == 1.0)
		{
			status->CrateBuff.SpeedMultiplier = pow_SpeedMultiplier;
			status->RecalculateStatus();
			R->AL(pTechno->GetOwningHouse()->IsInPlayerControl);
			return 0x483078;
		}
	}
	return 0x483081;
}

DEFINE_HOOK(0x48294F, CellClass_CrateBeingCollected_Cloak2, 6)
{
	GET(TechnoClass*, pTechno, EDX);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		status->CrateBuff.Cloakable = true;
		status->RecalculateStatus();
		return 0x482956;
	}
	return 0;
}
#pragma endregion

#pragma region Virtual Unit
DEFINE_HOOK(0x69251A, ScrollClass_ProcessClickCoords_VirtualUnit, 0x6)
{
	GET(AbstractClass*, pTarget, EAX);
	if (pTarget)
	{
		TechnoClass* pTechno = nullptr;
		if (CastToTechno(pTarget, pTechno))
		{
			if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
			{
				if (status->VirtualUnit || status->Disappear)
				{
					// 虚单位不可选择
					R->EAX(0);
				}
			}
		}
	}
	return 0;
}

DEFINE_HOOK(0x6DA3FF, TacticalClass_SelectAt_VirtualUnit, 0x6)
{
	GET(AbstractClass*, pTarget, EAX);
	if (pTarget)
	{
		TechnoClass* pTechno = nullptr;
		if (CastToTechno(pTarget, pTechno))
		{
			if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
			{
				if (status->VirtualUnit || status->Disappear)
				{
					// 虚单位不纳入可选择的范围
					return 0x6DA440;
				}
			}
		}
	}
	return 0;
}
#pragma endregion

#pragma region Show Building Range
DEFINE_HOOK(0x6D5116, TacticalClass_Draw_Placement_Recheck, 0x5)
{
	BuildingTypeClass* pBuildingType = DisplayClass::Display_PendingObject.get();
	if (pBuildingType->WhatAmI() == AbstractType::BuildingType)
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
	GET_STACK(bool, canBuild, 0x3C);
	if (!canBuild && CombatDamage::Data()->AllowUnitAsBaseNormal)
	{
		BuildingTypeClass* pBuildingType = nullptr;
		if ((!TechnoExt::BaseUnitArray.empty() || !TechnoExt::BaseStandArray.empty()) && (pBuildingType = DisplayClass::Display_PendingObject) != nullptr)
		{
			// 获取建筑建造范围四点坐标
			// 显示建造范围
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
			// 有效范围
			int minX = nCell.X;
			int minY = nCell.Y;
			int maxX = eCell.X;
			int maxY = wCell.Y;
			// 检查单位节点
			bool found = false;
			int houseIndex = DisplayClass::Display_PendingHouse;
			for (auto it : TechnoExt::BaseUnitArray)
			{
				found = CanBeBase(it.first, it.second, houseIndex, minX, maxX, minY, maxY);
				if (found)
				{
					break;
				}
			}
			if (!found && CombatDamage::Data()->AllowStandAsBaseNormal)
			{
				for (auto it : TechnoExt::BaseStandArray)
				{
					found = CanBeBase(it.first, it.second, houseIndex, minX, maxX, minY, maxY);
					if (found)
					{
						break;
					}
				}
			}
			if (found)
			{
				R->Stack(0x3C, true);
			}
		}
	}
	return 0;
}
#pragma endregion
