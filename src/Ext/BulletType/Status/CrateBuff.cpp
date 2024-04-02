#include "../BulletStatus.h"

#include "../Trajectory/StraightTrajectory.h"


void BulletStatus::OnUpdate_RecalculateStatus()
{
	if (!SpeedChanged)
	{
		// 时刻记录抛射体的状态
		_recordStatus.Health = pBullet->Health;
		_recordStatus.Speed = pBullet->Speed;
		_recordStatus.Velocity = pBullet->Velocity;
		_recordStatus.CourseLocked = pBullet->CourseLocked;
	}
	if (AttachEffect* aeManager = AEManager())
	{
		CrateBuffData aeBuff = aeManager->CountAttachStatusMultiplier();
		int newHealth = _recordStatus.Health;
		if (aeBuff.FirepowerMultiplier != 1)
		{
			newHealth = (int)(newHealth * aeBuff.FirepowerMultiplier);
		}
		// 重设伤害值
		pBullet->Health = newHealth;
		damage.Damage = newHealth;
		// 计算AE速度加成
		if (aeBuff.SpeedMultiplier != 1)
		{
			SpeedChanged = true;
		}
		// 还原
		if (SpeedChanged && aeBuff.SpeedMultiplier == 1.0)
		{
			SpeedChanged = false;
			LocationLocked = false;
			pBullet->Speed = _recordStatus.Speed;
			if (StraightTrajectory* straight = GetComponent<StraightTrajectory>())
			{
				straight->ResetStraightMissileVelocity();
			}
			else if (IsArcing())
			{
				// 抛物线类型的向量，只恢复方向向量，即X和Y
				double x = _recordStatus.Velocity.X;
				double y = _recordStatus.Velocity.Y;
				BulletVelocity nowVelocity = pBullet->Velocity;
				if (nowVelocity.X < 0 && x > 0)
				{
					x *= -1;
				}
				if (nowVelocity.Y < 0 && y > 0)
				{
					y *= -1;
				}
				pBullet->Velocity.X = x;
				pBullet->Velocity.Y = y;
			}
			return;
		}
		// 更改运动向量
		if (SpeedChanged)
		{
			double multiplier = aeBuff.SpeedMultiplier;
			if (multiplier == 0.0)
			{
				LocationLocked = true;
				pBullet->Speed = 1;
				multiplier = 1E-19;
			}
			// 导弹类需要每帧更改一次运动向量
			if (StraightTrajectory* straight = GetComponent<StraightTrajectory>())
			{
				// 直线导弹用保存的向量覆盖，每次都要重新计算
				pBullet->Velocity *= multiplier;
			}
			else if (IsArcing())
			{
				// Arcing类，重算方向上向量，即X和Y
				BulletVelocity recVelocity = _recordStatus.Velocity;
				recVelocity.Z = pBullet->Velocity.Z;
				BulletVelocity newVelocity = recVelocity * multiplier;
				pBullet->Velocity = newVelocity;
			}
			else
			{
				pBullet->Velocity *= multiplier;
			}

		}
	}
}
