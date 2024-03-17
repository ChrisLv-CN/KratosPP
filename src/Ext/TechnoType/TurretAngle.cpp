#include "TurretAngle.h"

#include <Extension/TechnoExt.h>

#include <Ext/Helper/DrawEx.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/TechnoType/TechnoStatus.h>

TurretAngleData* TurretAngle::GetTurretAngleData()
{
	if (!_data)
	{
		_data = INI::GetConfig<TurretAngleData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _data;
}

TechnoStatus* TurretAngle::GetTechnoStatue()
{
	if (!_status)
	{
		_status = _gameObject->GetComponent<TechnoStatus>();
	}
	return _status;
}

int TurretAngle::Dir2FacingIndex180to360(DirStruct dir)
{
	int dirIndex = Dir2FacingIndex(dir, 180) * 2;
	if (dirIndex > 0 && dirIndex < 180)
	{
		dirIndex++;
	}
	else if (dirIndex > 180)
	{
		dirIndex--;
	}
	return dirIndex;
}

bool TurretAngle::DefaultAngleIsChange(DirStruct bodyDir)
{
	// 车体朝向方向，游戏限制只能划180份
	int bodyDirIndex = Dir2FacingIndex180to360(bodyDir);
	DirStruct newDefaultDir;
	ChangeDefaultDir = TryGetDefaultAngle(bodyDirIndex, newDefaultDir);
	if (ChangeDefaultDir)
	{
		LockTurretDir = newDefaultDir;
	}
	return ChangeDefaultDir;
}

bool TurretAngle::TryGetDefaultAngle(int& bodyDirIndex, DirStruct& newDefaultDir)
{
	TurretAngleData* data = GetTurretAngleData();
	if (data->DefaultAngle > 0)
	{
		// 修改单位朝向指向虚拟方向
		bodyDirIndex += data->DefaultAngle;
		if (bodyDirIndex > 360)
		{
			bodyDirIndex -= 360;
		}
		newDefaultDir = DirNormalized(bodyDirIndex, 360);
		return true;
	}
	return false;
}
bool TurretAngle::InDeadZone(int bodyTargetDelta, int min, int max)
{
	return bodyTargetDelta > min && bodyTargetDelta < max;
}

void TurretAngle::BlockTurretFacing(DirStruct bodyDir, int bodyDirIndex, int min, int max, int bodyTargetDelta)
{
	// 炮塔卡在限位上，取最靠近的一边
	int targetAngle = GetTurnAngle(bodyTargetDelta, min, max) + bodyDirIndex;
	if (targetAngle > 360)
	{
		targetAngle -= 360;
	}
	// 目标和本体朝向的角度
	LockTurretDir = DirNormalized(targetAngle, 360);
	LockTurret = true;
	// 活区大于180，炮塔会从最近的位置转过去，穿过死区，强制转回前方绕过死区
	int angle = IncludedAngle360(bodyDirIndex, targetAngle);
	if (max - min <= 180)
	{
		ForceTurretToForward(bodyDir, bodyDirIndex, min, max, angle);
	}
}

bool TurretAngle::ForceTurretToForward(DirStruct bodyDir, int bodyDirIndex, int min, int max, int bodyTargetDelta)
{
	// 检查炮塔朝向角度和目标朝向角度的差值，判断是否需要转回前方
	DirStruct turretDir = pTechno->SecondaryFacing.Current();
	int turretDirIndex = Dir2FacingIndex180to360(turretDir);
	int turretAngle = IncludedAngle360(bodyDirIndex, turretDirIndex);

	if (turretAngle > 180)
	{
		if (InDeadZone(turretAngle, min, max))
		{
			// 目标也在左区，但是在死区范围内
			int turnAngle = GetTurnAngle(turretAngle, min, max) + bodyDirIndex;
			if (turnAngle > 360)
			{
				turnAngle -= 360;
			}
			// 逆时针回转到限位
			LockTurretDir = DirNormalized(turnAngle, 360);
			pTechno->SecondaryFacing.SetCurrent(LockTurretDir);
			return true;
		}
		else if (bodyTargetDelta < 180)
		{
			// 炮塔在左区，如果目标在右区，顺时针回转
			TurnToRight(turretAngle, bodyDirIndex, bodyDir);
			return true;
		}
	}
	else if (turretAngle > 0)
	{
		if (InDeadZone(turretAngle, min, max))
		{
			// 目标也在右区，但是在死区范围内
			// 逆时针回转到限位
			int turnAngle = GetTurnAngle(turretAngle, min, max) + bodyDirIndex;
			if (turnAngle > 360)
			{
				turnAngle -= 360;
			}
			LockTurretDir = DirNormalized(turnAngle, 360);
			pTechno->SecondaryFacing.SetCurrent(LockTurretDir);
			return true;
		}
		else if (bodyTargetDelta > 180)
		{
			// 炮塔在右区，如果目标在左区，逆时针回转
			TurnToLeft(turretAngle, bodyDirIndex, bodyDir);
			return true;
		}
	}
	return false;
}

void TurretAngle::TurnToLeft(int turretAngle, int bodyDirIndex, DirStruct bodyDir)
{
	// 逆时针回转
	int turnAngle = 0;
	if (turretAngle > 90)
	{
		turnAngle = turretAngle - 90 + bodyDirIndex;
		if (turnAngle > 360)
		{
			turnAngle -= 360;
		}
		LockTurretDir = DirNormalized(turnAngle, 360);
	}
	else
	{
		LockTurretDir = bodyDir;
	}
}

void TurretAngle::TurnToRight(int turretAngle, int bodyDirIndex, DirStruct bodyDir)
{
	// 顺时针回转
	int turnAngle = 0;
	if (360 - turretAngle > 90)
	{
		turnAngle = turretAngle + 90 + bodyDirIndex;
		if (turnAngle > 360)
		{
			turnAngle -= 360;
		}
		LockTurretDir = DirNormalized(turnAngle, 360);
	}
	else
	{
		LockTurretDir = bodyDir;
	}
}

bool TurretAngle::TryTurnBodyToAngle(DirStruct targetDir, int bodyDirIndex, int bodyTargetDelta)
{
	TechnoStatus* status = GetTechnoStatue();
	if (!IsBuilding() && !_isMoving && !pTechno->PrimaryFacing.IsRotating() && status
		&& (!status->AmIStand() || !status->MyStandData.Enable || !status->MyStandData.LockDirection || status->MyStandData.FreeDirection))
	{
		TurretAngleData* data = GetTurretAngleData();
		DirStruct turnDir = targetDir;
		// 启用侧舷接敌，计算侧舷角度和方位
		if (data->AutoTurn)
		{
			Point2D angleZone{};
			if (bodyTargetDelta >= 180)
			{
				// 目标在左区，正后方算左边
				angleZone = data->SideboardAngleL;
			}
			else
			{
				// 目标在右区，正前方算右边
				angleZone = data->SideboardAngleR;
			}
			// 目标角度在死区内，旋转
			if (bodyTargetDelta < angleZone.X || bodyTargetDelta > angleZone.Y)
			{
				int turnAngle = GetTurnAngle(bodyTargetDelta, angleZone);
				int turnDelta = 0;
				if (turnAngle < bodyTargetDelta)
				{
					// 侧舷的角度在目标的左边，顺时针转差值
					turnDelta = bodyTargetDelta - turnAngle;
				}
				else if (turnAngle > bodyTargetDelta)
				{
					// 侧舷的角度在目标的右边，逆时针转差值
					turnDelta = 360 - (turnAngle - bodyTargetDelta);
				}
				// 算实际世界坐标角度
				turnDelta += bodyDirIndex;
				if (turnDelta > 360)
				{
					turnDelta -= 360;
				}
				if (turnDelta > 0)
				{
					turnDir = DirNormalized(turnDelta, 360);
				}

				pTechno->PrimaryFacing.SetDesired(turnDir);
			}
		}
		else
		{
			pTechno->PrimaryFacing.SetDesired(turnDir);
		}
		return true;
	}
	return false;
}

void TurretAngle::Setup()
{
	_data = nullptr;
	_status = nullptr;
	if ((!IsUnit() && !IsBuilding()) || !GetTurretAngleData()->Enable || !pTechno->HasTurret() || pTechno->GetTechnoType()->TurretSpins)
	{
		Disable();
	}
}

void TurretAngle::Awake()
{
	Setup();
}

void TurretAngle::ExtChanged()
{
	Setup();
}

void TurretAngle::OnRemove()
{
	_isPut = false;
}

void TurretAngle::OnUpdate()
{
	if (!IsDeadOrInvisible(pTechno))
	{
		if (!_isPut)
		{
			_isPut = true;
			// 车体朝向方向
			DirStruct bodyDir = pTechno->PrimaryFacing.Current();
			LockTurretDir = bodyDir;
			ChangeDefaultDir = DefaultAngleIsChange(bodyDir);
			if (ChangeDefaultDir)
			{
				pTechno->SecondaryFacing.SetCurrent(LockTurretDir);
				return;
			}
		}
		FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
		_isMoving = pFoot->Locomotor->Is_Moving() && pFoot->GetCurrentSpeed() > 0;
		CoordStruct sourcePos = pTechno->GetCoords();
		// 车体朝向方向
		DirStruct bodyDir = pTechno->PrimaryFacing.Current();
		LockTurretDir = bodyDir;
		// 车体朝向方向，游戏限制只能划180份
		int bodyDirIndex = Dir2FacingIndex180to360(bodyDir);
		DirStruct newDefaultDir;
		ChangeDefaultDir = TryGetDefaultAngle(bodyDirIndex, newDefaultDir);
		if (ChangeDefaultDir)
		{
			LockTurretDir = newDefaultDir;
		}
		// 修改单位朝向指向虚拟方向，后面计算以该虚拟方向为正面
		bodyDir = LockTurretDir;
		// 攻击目标或者移动目标存在，指向
		AbstractClass* pTarget = pTechno->Target;
		bool hasTarget = pTarget != nullptr;
		if (!hasTarget)
		{
			pTarget = dynamic_cast<FootClass*>(pTechno)->Destination;
		}
		if (pTarget)
		{
			// 目标所在方向
			DirStruct targetDir = pTechno->Direction(pTarget);
			// 游戏限制只能划180份，Index * 2
			int targetDirIndex = Dir2FacingIndex180to360(targetDir);
			// 取夹角的度数值
			int bodyTargetDelta = IncludedAngle360(bodyDirIndex, targetDirIndex);
			// 目标在射程范围内
			bool isCloseEnough = pTechno->IsCloseEnoughToAttack(pTarget);
			TurretAngleData* data = GetTurretAngleData();
			// 启用侧舷接敌
			if (hasTarget && isCloseEnough && data->AutoTurn)
			{
				TryTurnBodyToAngle(targetDir, bodyDirIndex, bodyTargetDelta);
			}
			// 启用炮塔限界
			if (data->AngleLimit)
			{
				// 判断是否在死区内
				int min = data->Angle.X;
				int max = data->Angle.Y;

				if (InDeadZone(bodyTargetDelta, min, max))
				{
					// 在死区内
					DeathZoneAction action = data->Action;
					// 移动前往目的地非攻击目标，改变策略卡住炮塔朝向
					if ((!hasTarget || !isCloseEnough) && ChangeDefaultDir)
					{
						action = DeathZoneAction::BLOCK;
					}
					// Logger.Log($"{Game.CurrentFrame} 旋转角度 {delta} 在死区 {turretAngleData.Angle} 内");
					switch (action)
					{
					case DeathZoneAction::BLOCK:
						BlockTurretFacing(bodyDir, bodyDirIndex, min, max, bodyTargetDelta);
						break;
					case DeathZoneAction::TURN:
						BlockTurretFacing(bodyDir, bodyDirIndex, min, max, bodyTargetDelta);
						if (isCloseEnough)
						{
							// 转动车身朝向目标
							TryTurnBodyToAngle(targetDir, bodyDirIndex, bodyTargetDelta);
						}
						break;
					default:
						ClearAllTarget(pTechno);
						break;
					}
				}
				else
				{
					// 不在死区，但是如果死区过小，炮塔会从最近的位置转过去，穿过死区，强制转回前方绕过死区
					int range = max - min;
					if (range > 0 && range <= 180)
					{
						LockTurret = ForceTurretToForward(bodyDir, bodyDirIndex, min, max, bodyTargetDelta);
					}
					else
					{
						LockTurret = false;
					}
				}
			}
		}
	}
	else
	{
		LockTurret = false;
		_isMoving = false;
	}
}

