#include "../BulletStatus.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

void BulletStatus::OnUpdate_Trajectory_Arcing()
{
	ResetArcingVelocity();
}

void BulletStatus::ResetArcingVelocity(float speedMultiple, bool force)
{
	if (force || !_arcingTrajectoryInitFlag && trajectoryData->AdvancedBallistics)
	{
		_arcingTrajectoryInitFlag = true;
		CoordStruct sourcePos = pBullet->GetCoords();
		CoordStruct targetPos = pBullet->TargetCoords;

		if (trajectoryData->ArcingFixedSpeed > 0)
		{
			pBullet->Speed = trajectoryData->ArcingFixedSpeed;
		}
		else
		{
			pBullet->Speed += pBullet->Type->Acceleration;
		}

		int speed = (int)(pBullet->Speed * speedMultiple);
		int gravity = RulesClass::Instance->Gravity;
		if (trajectoryData->Gravity > 0)
		{
			gravity = trajectoryData->Gravity;
		}
		bool lobber = pBullet->WeaponType ? pBullet->WeaponType->Lobber : false;
		bool inaccurate = trajectoryData->Inaccurate;
		float min = trajectoryData->BallisticScatterMin;
		float max = trajectoryData->BallisticScatterMax;

		double straightDistance = 0;
		double realSpeed = 0;
		CellClass* pTargetCell = nullptr;
		BulletVelocity velocity = GetBulletArcingVelocity(sourcePos, targetPos,
			speed, gravity, lobber, inaccurate, min, max,
			ToCoordStruct(pBullet->Velocity).Z, straightDistance, realSpeed, pTargetCell);
		pBullet->Speed = (int)realSpeed;
		pBullet->Velocity = velocity;
		pBullet->TargetCoords = targetPos;
		if (inaccurate && pTargetCell)
		{
			pBullet->Target = pTargetCell;
		}

	}
}

#pragma region Bounce Arcing
void BulletStatus::InitState_Bounce()
{
	if (!_isBounceSplit && IsArcing())
	{
		BounceData* data = INI::GetConfig<BounceData>(INI::Rules, pBullet->Type->ID)->Data;
		if (data->Enable)
		{
			BounceState.Enable(*data);
		}
	}
}

void BulletStatus::OnUpdate_Trajectory_Bounce()
{
	if (BounceState.IsActive() && !_isBounceSplit)
	{
		// 缓存弹跳的设置，在抛射体爆炸后，传递给子抛射体
		_bounceData = BounceState.Data;
	}
	if (_isBounceSplit || BounceState.IsActive())
	{
		CoordStruct location = pBullet->GetCoords();
		if (pBullet->GetHeight() > 0)
		{
			// 检查撞悬崖反弹
			CoordStruct nextPos = location + ToCoordStruct(pBullet->Velocity);
			CoordStruct nextCellPos = CoordStruct::Empty;
			bool onBridge = false;
			switch (CanMoveTo(location, nextPos, true, nextCellPos, onBridge))
			{
			case PassError::HITWALL:
				// 重设预定目标位置
				CoordStruct v = pBullet->TargetCoords - location;
				v.X *= -1;
				v.Y *= -1;
				CoordStruct newTargetPos = location + v;
				// 反弹
				pBullet->Velocity.X *= -1;
				pBullet->Velocity.Y *= -1;
				pBullet->SourceCoords = location;
				pBullet->TargetCoords = newTargetPos;
				break;
			}
		}
	}
}

bool BulletStatus::OnDetonate_Bounce(CoordStruct* pCoords)
{
	if (BounceState.IsActive() && !_isBounceSplit)
	{
		// 缓存弹跳的设置，在抛射体爆炸后，传递给子抛射体
		_bounceData = BounceState.Data;
	}
	return SpawnSplitCannon();
}

void BulletStatus::SetBounceData(BounceData bounceData)
{
	BounceData* data = INI::GetConfig<BounceData>(INI::Rules, pBullet->Type->ID)->Data;
	if (data->Enable)
	{
		_bounceData = *data;
	}
	else
	{
		_bounceData = bounceData;
	}
}

bool BulletStatus::SpawnSplitCannon()
{
	if ((BounceState.IsActive() || _isBounceSplit) && _bounceData.Enable
		&& (_bounceData.Times < 0 || _bounceIndex < _bounceData.Times)
		&& (_bounceData.Chance >= 1 || Bingo(_bounceData.Chance))
		)
	{
		CoordStruct sourcePos = pBullet->SourceCoords;
		CoordStruct explodePos = pBullet->TargetCoords;
		CoordStruct tempSourcePos = sourcePos;
		tempSourcePos.Z = 0;
		CoordStruct tempExplodePos = explodePos;
		tempExplodePos.Z = 0;
		// 飞行距离
		double dist = tempSourcePos.DistanceFrom(tempExplodePos);
		// Logger.Log($"{Game.CurrentFrame} [{section}]{pBullet} 初始位置{tempSourcePos}，爆炸位置{tempExplodePos}，飞行距离{dist}");
		if (dist > 0 && !isnan(dist) && !isinf(dist))
		{
			double next = dist * _bounceData.Elasticity; // 子抛射体弹跳的距离
			double speed = pBullet->WeaponType ? pBullet->WeaponType->GetSpeed((int)next) : pBullet->Speed;
			// Logger.Log($"{Game.CurrentFrame} [{section}]{pBullet} 初始位置{tempSourcePos}，爆炸位置{tempExplodePos}，子抛射体距离是 {dist} * {bounceData.Elasticity} = {next}，飞行速度{speed}，限制距离 {bounceData.Limit}");
			// 往前飞一半的距离
			if (next > Math::max(_bounceData.Limit + 1, speed))
			{
				// 检查地形，能不能跳
				if (CellClass* pCell = MapClass::Instance->TryGetCellAt(explodePos))
				{
					// 地形
					LandType landType = LandType::Clear;
					TileType tileType = TileType::Unknown;
					bool rebound = false; // 反弹
					if (_bounceData.IsOnLandType(pCell, landType) && _bounceData.IsOnTileType(pCell, tileType) && !_bounceData.Stop(pCell, rebound))
					{
						// 在允许的瓷砖类型内
						WeaponTypeClass* pWeapon = pBullet->WeaponType;
						std::string weaponId = _bounceData.Weapon;
						if (IsNotNone(weaponId))
						{
							pWeapon = WeaponTypeClass::Find(weaponId.c_str());
						}
						if (pWeapon)
						{
							// 取子武器的发射位置
							// 发射位置离地2个重力的高，不然射不出
							CoordStruct nextSourcePos = pBullet->GetCoords(); // 子抛射体从当前位置射出
							nextSourcePos.Z = pCell->GetCoordsWithBridge().Z;
							nextSourcePos.Z += RulesClass::Instance->Gravity * 2;
							CoordStruct nextTargetPos = CoordStruct::Empty;
							// 根据地形削减或加强弹性
							std::string landId{ "" };
							for (auto it = LandTypeStrings.begin(); it != LandTypeStrings.end(); it++)
							{
								if (it->second == landType)
								{
									landId = it->first;
									break;
								}
							}
							if (IsNotNone(landId))
							{
								next *= INI::GetConfig<BounceLandData>(INI::Rules, landId.c_str())->Data->Elasticity;
							}
							// 根据地形和地块获取弹性指数和方向，获得子武器的目的地
							switch (tileType)
							{
							case TileType::Cliff:
							case TileType::DestroyableCliff:
								// 上坡还是下坡
								// 反弹
								// nextTargetPos = GetForwardCoords(tempSourcePos, tempExplodePos, dist - next, dist);
								break;
							case TileType::Ramp:
								// 斜坡，弹的更高
								// next *= 0.5;
								// _bounceSpeedMultiple *= 0.5f;
								break;
							}
							// 再次确认削减之后的距离还可以跳
							if (next > Math::max(_bounceData.Limit + 1, speed))
							{
								if (nextTargetPos.IsEmpty())
								{
									// 目标位置在炮口与目标连线的延长线外next的距离
									if (rebound)
									{
										// 翻转方向
										nextTargetPos = GetForwardCoords(tempExplodePos, tempSourcePos, next, dist);
									}
									else
									{
										nextTargetPos = GetForwardCoords(tempSourcePos, tempExplodePos, dist + next, dist);
									}
								}
								// Logger.Log($"{Game.CurrentFrame} [{section}]{pBullet} 延线取距离 {dist} + {next} = {dist + next} 的点 {nextTargetPos}");
								// 不能在同一格里跳，不然会直接判定为命中撞爆
								if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(nextTargetPos))
								{
									if (pTargetCell != pCell)
									{
										CoordStruct cellPos = pTargetCell->GetCoordsWithBridge();
										nextTargetPos.Z = cellPos.Z;
										AbstractClass* pTarget = pTargetCell;
										BulletClass* pNewBullet = FireBulletTo(nullptr, pSource, pTarget, pSourceHouse, pWeapon, nextSourcePos, nextTargetPos);
										// Logger.Log($"{Game.CurrentFrame} [{section}]{pBullet} 跳弹已发射 {pNewBullet}，发射位置 {nextSourcePos}，目标位置 {nextTargetPos}, 两点距离 {nextTargetPos.DistanceFrom(nextSourcePos)}");
										if (pNewBullet)
										{
											if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pNewBullet))
											{
												status->ResetArcingVelocity(_bounceSpeedMultiple);
												status->SetBounceData(_bounceData);
												status->_isBounceSplit = true;
												status->_bounceIndex = _bounceIndex + 1;
												status->_bounceTargetPos = nextTargetPos;
												status->_bounceSpeedMultiple = _bounceSpeedMultiple;
											}
											// 播放动画
											if (IsNotNone(_bounceData.ExpireAnim))
											{
												AnimTypeClass* pAnimType = AnimTypeClass::Find(_bounceData.ExpireAnim.c_str());
												if (pAnimType)
												{
													AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, nextSourcePos);
													pAnim->Owner = pSourceHouse;
												}
											}
										}
										return !_bounceData.ExplodeOnHit;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
#pragma endregion
