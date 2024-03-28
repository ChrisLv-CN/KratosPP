#include "InfoEffect.h"

#include <JumpjetLocomotionClass.h>

#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/Common/PrintTextManager.h>

void InfoEffect::OnGScreenRenderEnd(CoordStruct location)
{
	if (!pObject->InLimbo)
	{
		AttachEffect* aem = AE->AEManager;
		HouseClass* pHouse = AE->pSourceHouse;
		ColorStruct houseColor = pHouse != nullptr ? pHouse->LaserColor : Colors::Empty;
		bool isPlayerControl = pHouse != nullptr && pHouse->IsInPlayerControl;
		bool isSelected = pObject->IsSelected;

		CoordStruct sourcePos = location;
		Point2D pos = ToClientPos(sourcePos);

		// 需要遍历读取具体的AE状态的信息的部分统一读取
		// 显示Duration和InitDelay
		bool checkDuration = Data->Duration.Mode != InfoMode::NONE && IsNotNone(Data->Duration.Watch) && (Data->Duration.ShowEnemy || isPlayerControl) && (!Data->Duration.OnlySelected || isSelected);
		bool checkInitDelay = Data->InitDelay.Mode != InfoMode::NONE && IsNotNone(Data->InitDelay.Watch) && (Data->InitDelay.ShowEnemy || isPlayerControl) && (!Data->InitDelay.OnlySelected || isSelected);
		if (checkDuration || checkInitDelay)
		{
			// 循环遍历AE
			int duration = -1;
			int initDelay = -1;
			auto data = Data;
			aem->ForeachChild([&checkDuration, &checkInitDelay, &duration, &initDelay, &data](Component* c) {
				if (AttachEffectScript* ae = dynamic_cast<AttachEffectScript*>(c))
				{
					std::string aeName = ae->AEData.Name;
					// 读取Duration
					int durationLeft = -1;
					if (checkDuration && aeName == data->Duration.Watch && ae->TryGetDurationTimeLeft(durationLeft))
					{
						switch (data->Duration.Sort)
						{
						case SortType::MIN:
							if (durationLeft < duration)
							{
								duration = durationLeft;
							}
							break;
						case SortType::MAX:
							if (durationLeft > duration)
							{
								duration = durationLeft;
							}
							break;
						default:
							// 找到目标后，停止检查
							checkDuration = false;
							break;
						}
						if (duration < 0)
						{
							duration = durationLeft;
						}
					}
					// 读取InitDelay
					int delayLeft = -1;
					if (checkInitDelay && aeName == data->InitDelay.Watch && ae->TryGetInitDelayTimeLeft(delayLeft))
					{
						switch (data->InitDelay.Sort)
						{
						case SortType::MIN:
							if (delayLeft < initDelay)
							{
								initDelay = delayLeft;
							}
							break;
						case SortType::MAX:
							if (delayLeft > initDelay)
							{
								initDelay = delayLeft;
							}
							break;
						default:
							// 找到目标后，停止检查
							checkInitDelay = false;
							break;
						}
						if (initDelay < 0)
						{
							initDelay = delayLeft;
						}
					}
					// 都不需要继续查找，跳出AE遍历
					if (!checkDuration && !checkInitDelay)
					{
						ae->IsBreak();
					}
				}
				});

			// 显示查找到的信息
			if (duration > -1)
			{
				// 显示Duration
				PrintInfoNumber(duration, houseColor, pos, Data->Duration);
			}
			if (initDelay > -1)
			{
				// 显示InitDelay
				PrintInfoNumber(duration, houseColor, pos, Data->InitDelay);
			}
		}

		// 不需要遍历AE清单，可以直接从AE管理器获得的信息
		// 显示Delay
		if (Data->Delay.Mode != InfoMode::NONE && IsNotNone(Data->Delay.Watch) && (Data->Delay.ShowEnemy || isPlayerControl) && (!Data->Delay.OnlySelected || isSelected))
		{
			int delay = -1;
			auto it = aem->DisableDelayTimers.find(Data->Delay.Watch);
			if (it != aem->DisableDelayTimers.end())
			{
				TimerStruct timer = it->second;
				if (timer.InProgress())
				{
					delay = timer.GetTimeLeft();
				}
			}
			if (delay > -1)
			{
				// 显示delay
				PrintInfoNumber(delay, houseColor, pos, Data->Delay);
			}
		}
		// 显示Stack
		if (Data->Stack.Mode != InfoMode::NONE && IsNotNone(Data->Stack.Watch) && (Data->Stack.ShowEnemy || isPlayerControl) && (!Data->Stack.OnlySelected || isSelected))
		{
			int stacks = -1;
			auto it = aem->AEStacks.find(Data->Stack.Watch);
			if (it != aem->AEStacks.end())
			{
				stacks = it->second;
			}
			if (stacks > -1)
			{
				// 显示Stacks
				PrintInfoNumber(stacks, houseColor, pos, Data->Stack);
			}
		}

		// 显示附着对象的信息
		RectangleStruct bounds = DSurface::Temp->GetRect();
		bounds.Height -= 34;

		// 显示单位信息
		if (pTechno)
		{
			// 显示血量
			if (Data->Health.Mode != InfoMode::NONE && (Data->Health.ShowEnemy || isPlayerControl) && (!Data->Health.OnlySelected || isSelected))
			{
				int health = -1;
				if ((health = pTechno->Health) > 0)
				{
					PrintInfoNumber(health, houseColor, pos, Data->Health);
				}
			}
			// 显示弹药
			if (Data->Ammo.Mode != InfoMode::NONE && (Data->Ammo.ShowEnemy || isPlayerControl) && (!Data->Ammo.OnlySelected || isSelected))
			{
				int ammo = -1;
				if (pTechno->GetTechnoType()->Ammo > 0)
				{
					ammo = pTechno->Ammo;
				}
				if (ammo > 0)
				{
					PrintInfoNumber(ammo, houseColor, pos, Data->Ammo);
				}
			}
			// 显示装填时间
			if (Data->Reload.Mode != InfoMode::NONE && (Data->Reload.ShowEnemy || isPlayerControl) && (!Data->Reload.OnlySelected || isSelected))
			{
				int delay = -1;
				TimerStruct timer = pTechno->ReloadTimer;
				if (timer.InProgress())
				{
					delay = timer.GetTimeLeft();
				}
				if (delay > -1)
				{
					PrintInfoNumber(delay, houseColor, pos, Data->Ammo);
				}
			}
			// 显示ROF时间
			if (Data->ROF.Mode != InfoMode::NONE && (Data->ROF.ShowEnemy || isPlayerControl) && (!Data->ROF.OnlySelected || isSelected))
			{
				int delay = -1;
				TimerStruct timer = pTechno->ROFTimer;
				if (timer.InProgress())
				{
					delay = timer.GetTimeLeft();
				}
				if (delay > -1)
				{
					PrintInfoNumber(delay, houseColor, pos, Data->Ammo);
				}
			}

			// 写字
			// 显示Armor
			if (Data->Armor.Mode != InfoMode::NONE && (Data->Armor.ShowEnemy || isPlayerControl) && (!Data->Armor.OnlySelected || isSelected))
			{
				Armor armor = pObject->GetType()->Armor;
				std::string armorName = WarheadTypeExt::TypeData::GetArmorName(armor);
				PrintInfoText(armorName, houseColor, pos, Data->Armor);
			}
			// 显示Mission
			if (Data->Mission.Mode != InfoMode::NONE && (Data->Mission.ShowEnemy || isPlayerControl) && (!Data->Mission.OnlySelected || isSelected))
			{
				Mission mission = pObject->GetCurrentMission();
				std::string text = std::to_string((int)mission);
				for (auto it : MissionTypeStrings)
				{
					if (it.second == mission)
					{
						text = it.first;
						break;
					}
				}
				PrintInfoText(text, houseColor, pos, Data->Mission);
			}

			// 画线
			// 显示移动目标连线
			if (Data->Dest.Mode != InfoMode::NONE && (Data->Dest.ShowEnemy || isPlayerControl) && (!Data->Dest.OnlySelected || isSelected))
			{
				AbstractClass* pDest = nullptr;
				if (IsFoot() && (pDest = dynamic_cast<FootClass*>(pTechno)->Destination) != nullptr)
				{
					CoordStruct targetPos = pDest->GetCoords();
					DrawDashedLine(DSurface::Temp, pos, ToClientPos(targetPos), Data->Dest.Color, bounds);
				}
			}
			// 显示单位朝向
			if (Data->BodyDir.Mode != InfoMode::NONE && (Data->BodyDir.ShowEnemy || isPlayerControl) && (!Data->BodyDir.OnlySelected || isSelected))
			{
				DirStruct dir = pTechno->PrimaryFacing.Current();
				DirStruct toDir = pTechno->PrimaryFacing.Desired();
				if (pTechno->GetTechnoType()->Locomotor == LocomotionClass::CLSIDs::Jumpjet)
				{
					FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
					if (JumpjetLocomotionClass* jjLoco = dynamic_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get()))
					{
						dir = jjLoco->LocomotionFacing.Current();
						toDir = jjLoco->LocomotionFacing.Desired();
					}
				}
				CoordStruct flh{ 1024, 0, 0 };
				CoordStruct targetPos = GetFLHAbsoluteCoords(sourcePos, flh, dir);
				DrawLine(DSurface::Temp, pos, ToClientPos(targetPos), Data->BodyDir.Color, bounds);
				if (toDir != dir)
				{
					CoordStruct targetToPos = GetFLHAbsoluteCoords(sourcePos, flh, toDir);
					DrawDashedLine(DSurface::Temp, pos, ToClientPos(targetToPos), Data->BodyDir.Color, bounds);
				}
			}
			// 显示单位炮塔朝向
			if (Data->TurretDir.Mode != InfoMode::NONE && (Data->TurretDir.ShowEnemy || isPlayerControl) && (!Data->TurretDir.OnlySelected || isSelected))
			{
				DirStruct dir = pTechno->TurretFacing().Current();
				DirStruct toDir = pTechno->TurretFacing().Desired();
				CoordStruct flh{ 1024, 0, 0 };
				CoordStruct targetPos = GetFLHAbsoluteCoords(sourcePos, flh, dir);
				DrawLine(DSurface::Temp, pos, ToClientPos(targetPos), Data->TurretDir.Color, bounds);
				if (toDir != dir)
				{
					CoordStruct targetToPos = GetFLHAbsoluteCoords(sourcePos, flh, toDir);
					DrawDashedLine(DSurface::Temp, pos, ToClientPos(targetToPos), Data->BodyDir.Color, bounds);
				}
			}
		}

		// 写字
		// 显示ID
		if (Data->ID.Mode != InfoMode::NONE && (Data->ID.ShowEnemy || isPlayerControl) && (!Data->ID.OnlySelected || isSelected))
		{
			std::string id = pObject->GetType()->ID;
			PrintInfoText(id, houseColor, pos, Data->ID);
		}

		// 画线
		// 显示目标连线
		if (Data->Target.Mode != InfoMode::NONE && (Data->Target.ShowEnemy || isPlayerControl) && (!Data->Target.OnlySelected || isSelected))
		{
			AbstractClass* pTarget = nullptr;
			if (pTechno)
			{
				pTarget = pTechno->Target;
			}
			else if (pBullet)
			{
				pTarget = pBullet->Target;
			}
			if (pTarget)
			{
				CoordStruct sourceLocation = location;
				if (pTechno)
				{
					int weaponIdx = pTechno->SelectWeapon(pTarget);
					sourceLocation = pTechno->GetFLH(weaponIdx, CoordStruct::Empty);
				}
				CoordStruct targetLocation = pTarget->GetCoords();
				DrawTargetLaser(DSurface::Temp, sourceLocation, targetLocation, Data->Target.Color, bounds);
				if (Data->Target.Mode == InfoMode::TEXT && pTarget->AbstractFlags & AbstractFlags::Object)
				{
					Point2D targetPos = ToClientPos(targetLocation);
					std::string id = dynamic_cast<ObjectClass*>(pTarget)->GetType()->ID;
					PrintInfoText(id, Data->Target.Color, targetPos, Data->Target);
				}
			}
		}

		// 显示单位位置
		if (Data->Location.Mode != InfoMode::NONE && (Data->Location.ShowEnemy || isPlayerControl) && (!Data->Location.OnlySelected || isSelected))
		{
			DrawCrosshair(DSurface::Temp, sourcePos, 128, Data->Location.Color, bounds, false);
		}
		// 显示单位所在的格子
		if (Data->Cell.Mode != InfoMode::NONE && (Data->Cell.ShowEnemy || isPlayerControl) && (!Data->Cell.OnlySelected || isSelected))
		{
			if (CellClass* pCell = MapClass::Instance()->TryGetCellAt(sourcePos))
			{
				CoordStruct targetPos = pCell->GetCoordsWithBridge();
				DrawCell(DSurface::Temp, targetPos, Data->Cell.Color, bounds, false);
				DrawLine(DSurface::Temp, pos, ToClientPos(targetPos), Data->Cell.Color, bounds);
				if (sourcePos.Z != targetPos.Z)
				{
					CoordStruct pos2 = sourcePos;
					pos2.Z = targetPos.Z;
					// 单位坐标和地面映射位置
					DrawDashedLine(DSurface::Temp, pos, ToClientPos(pos2), Data->Cell.Color, bounds);
					// 单位坐标地面映射和格子的差
					DrawDashedLine(DSurface::Temp, ToClientPos(targetPos), ToClientPos(pos2), Data->Cell.Color, bounds);
				}
			}
		}
	}
}

void InfoEffect::PrintInfoNumber(int number, ColorStruct houseColor, Point2D location, InfoEntity data)
{
	// 调整锚点
	Point2D pos = location;
	pos.X += data.Offset.X; // 锚点向右的偏移值
	pos.Y += data.Offset.Y; // 锚点向下的偏移值

	// 修正锚点
	if (!data.UseSHP)
	{
		// 根据对其方式修正锚点
		OffsetAlign(pos, std::to_wstring(number), data);
	}
	PrintTextManager::PrintNumber(number, houseColor, pos, data);
}

void InfoEffect::PrintInfoText(std::string text, ColorStruct houseColor, Point2D location, InfoEntity data)
{
	// 调整锚点
	Point2D pos = location;
	pos.X += data.Offset.X; // 锚点向右的偏移值
	pos.Y += data.Offset.Y; // 锚点向下的偏移值

	// 修正锚点
	if (!data.UseSHP)
	{
		// 根据对其方式修正锚点
		OffsetAlign(pos, String2WString(text), data);
	}
	PrintTextManager::PrintText(text, houseColor, pos, data);
}

void InfoEffect::OffsetAlign(Point2D& pos, std::wstring text, InfoEntity data)
{
	// 使用文字显示数字，文字的锚点在左上角
	// 重新调整锚点位置，向上抬起半个字的高度
	pos.Y = pos.Y - PrintTextManager::GetFontSize().Y / 2; // 字是20格高，上4中9下7
	// 按照文字对齐方式修正X的偏移值
	RectangleStruct textRect = Drawing::GetTextDimensions(text.c_str(), { 0, 0 }, 0, 2, 0);
	int width = textRect.Width;
	switch (data.Align)
	{
	case PrintTextAlign::CENTER:
		pos.X -= width / 2;
		break;
	case PrintTextAlign::RIGHT:
		pos.X -= width;
		break;
	}
}

