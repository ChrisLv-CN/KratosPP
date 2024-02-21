#include "Bounce.h"

#include "BulletStatus.h"
#include "Trajectory/ArcingTrajectory.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

void Bounce::SetBounceData(BounceData newData)
{
	_bounceData = newData;
	_isBounceSplit = true;
}

bool Bounce::SpawnSplitCannon()
{
	if (_bounceData.Enable
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
								// 不能在同一格里跳，不然会直接判定为命中撞爆
								if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(nextTargetPos))
								{
									if (pTargetCell != pCell)
									{
										BulletStatus* status = dynamic_cast<BulletStatus*>(_parent);
										CoordStruct cellPos = pTargetCell->GetCoordsWithBridge();
										nextTargetPos.Z = cellPos.Z;
										AbstractClass* pTarget = pTargetCell;
										BulletClass* pNewBullet = FireBulletTo(nullptr, status->pSource, pTarget, status->pSourceHouse, pWeapon, nextSourcePos, nextTargetPos);
										if (pNewBullet)
										{
											// 新弹道控制
											if (ArcingTrajectory* at = GetScript<BulletExt, ArcingTrajectory>(pNewBullet))
											{
												at->ResetArcingVelocity(_bounceSpeedMultiple);
												at->CanBounce = true;
											}
											// 新弹跳控制
											if (Bounce* b = GetScript<BulletExt, Bounce>(pNewBullet))
											{
												b->SetBounceData(_bounceData);
												b->_bounceIndex = _bounceIndex + 1;
												b->_bounceTargetPos = nextTargetPos;
												b->_bounceSpeedMultiple = _bounceSpeedMultiple;
											}
											// 播放动画
											if (IsNotNone(_bounceData.ExpireAnim))
											{
												AnimTypeClass* pAnimType = AnimTypeClass::Find(_bounceData.ExpireAnim.c_str());
												if (pAnimType)
												{
													AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, nextSourcePos);
													pAnim->Owner = status->pSourceHouse;
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

void Bounce::OnPut(CoordStruct* pCoord, DirType dirType)
{
	if (!_isBounceSplit && IsArcing())
	{
		BounceData* data = INI::GetConfig<BounceData>(INI::Rules, pBullet->Type->ID)->Data;
		if (data->Enable)
		{
			_bounceData = *data;
		}
	}
	// 抛射体不能跳，结束
	if (!_bounceData.Enable)
	{
		Disable();
	}
}

void Bounce::OnDetonate(CoordStruct* pCoords, bool& skip)
{
	skip = SpawnSplitCannon();
}

