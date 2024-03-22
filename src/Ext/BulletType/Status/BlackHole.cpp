#include "../BulletStatus.h"

#include <Ext/Helper/Physics.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

void BulletStatus::BlackHoleCapture(ObjectClass* pBlackHole, BlackHoleData data)
{
	if (!CaptureByBlackHole || !_blackHoleData.Enable || _blackHoleData.Weight < data.Weight)
	{
		CaptureByBlackHole = true;
		_pBlackHole = pBlackHole;
		_blackHoleData = data;
	}
}

void BulletStatus::BlackHoleCancel()
{
	if (CaptureByBlackHole && !IsDeadOrInvisible(pBullet, this))
	{
		pBullet->SourceCoords = pBullet->GetCoords();
		// Arcing摔地上，导弹不管
		if (IsArcing() || IsBomb())
		{
			pBullet->Velocity = BulletVelocity::Empty;
		}
		else
		{
			RecalculateBulletVelocity(pBullet);
		}
	}
	CaptureByBlackHole = false;
	_pBlackHole = nullptr;
	_blackHoleData.Enable = false;
}

void BulletStatus::OnUpdate_BlackHole()
{
	if (CaptureByBlackHole)
	{
		CoordStruct location = pBullet->GetCoords();
		BlackHoleState* blackHoleState = nullptr;
		AttachEffect* aem = AEManager();
		if (!_pBlackHole
			|| !TryGetBlackHoleState(_pBlackHole, blackHoleState)
			|| !blackHoleState->IsAlive()
			|| blackHoleState->IsOutOfRange(location)
			|| !aem || !aem->IsOnMark(_blackHoleData))
		{
			BlackHoleCancel();
		}
		else
		{
			CoordStruct blackHolePos = _pBlackHole->GetCoords();
			if (_blackHoleData.ChangeTarget)
			{
				pBullet->SetTarget(_pBlackHole);
				pBullet->TargetCoords = blackHolePos;
			}
			// 加上偏移值
			blackHolePos += _blackHoleData.Offset;
			// 获取一个从黑洞位置朝向预设目标位置的向量，该向量控制导弹的弹体朝向
			if (IsArcing() || IsBomb())
			{
				// 从当前位置朝向黑洞
				CoordStruct sourcePos = pBullet->GetCoords();
				pBullet->Velocity = GetBulletVelocity(sourcePos, blackHolePos);
			}
			else
			{
				// 从黑洞朝向预设目标位置
				pBullet->Velocity = GetBulletVelocity(blackHolePos, pBullet->TargetCoords);
			}
			// 黑洞伤害，排除我自己不是黑洞
			if (_blackHoleData.AllowDamageBullet && _blackHoleData.Damage != 0 && !BlackHole->IsAlive())
			{
				if (_blackHoleDamageDelay.Expired())
				{
					_blackHoleDamageDelay.Start(_blackHoleData.DamageDelay);
					TakeDamage(_blackHoleData.Damage, false, false, true);
				}
			}
		}
	}
}

void BulletStatus::OnUpdateEnd_BlackHole(CoordStruct& sourcePos)
{
	if (CaptureByBlackHole)
	{
		// 强行移动导弹的位置
		CoordStruct targetPos = GetFLHAbsoluteCoords(_pBlackHole, _blackHoleData.Offset, _blackHoleData.IsOnTurret);
		CoordStruct nextPos = targetPos;
		double dist = targetPos.DistanceFrom(sourcePos);
		// 获取捕获速度
		int speed = _blackHoleData.GetCaptureSpeed(1);
		if (dist > speed)
		{
			nextPos = GetForwardCoords(sourcePos, targetPos, speed);
		}
		// 抛射体撞到地面
		bool canMove = pBullet->GetHeight() > 0;
		// 检查悬崖
		if (canMove)
		{
			CoordStruct nextCellPos = CoordStruct::Empty;
			bool onBridge = false;
			switch (CanMoveTo(sourcePos, nextPos, _blackHoleData.AllowPassBuilding, nextCellPos, onBridge))
			{
			case PassError::UNDERGROUND:
			case PassError::HITWALL:
			case PassError::HITBUILDING:
			case PassError::DOWNBRIDGE:
			case PassError::UPBRIDEG:
				canMove = false;
				break;
			}
		}
		if (!canMove)
		{
			// 原地爆炸
			TakeDamage();
		}
		else
		{
			// 被黑洞吸走
			pBullet->SetLocation(nextPos);
			// 修改了位置所以更新位置
			sourcePos = nextPos;
		}
	}
}
