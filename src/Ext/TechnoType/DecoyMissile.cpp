#include "DecoyMissile.h"

#include <Extension/BulletExt.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/BulletType/Trajectory/MissileTrajectory.h>

DecoyMissileData* DecoyMissile::GetDecoyMissileData()
{
	if (!_data)
	{
		_data = INI::GetConfig<DecoyMissileData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _data;
}

bool DecoyMissile::DropOne(WeaponTypeClass* pWeapon)
{
	if (_bullets < 0)
	{
		_bullets = pWeapon->Burst;
	}
	if (_reloadTimer.Expired() && _delayTimer.Expired())
	{
		if (--_bullets >= 0)
		{
			_delayTimer.Start(GetDecoyMissileData()->Delay);
			return true;
		}
		Reload(pWeapon);
	}
 	return false;
}

void DecoyMissile::Reload(WeaponTypeClass* pWeapon)
{
	_bullets = pWeapon->Burst;
	_reloadTimer.Start(pWeapon->ROF);
	_delayTimer.Stop();
	_fire = GetDecoyMissileData()->AlwaysFire;
}

void DecoyMissile::ClearDeadDecoy()
{
	for (auto it = _decoys.begin(); it != _decoys.end();)
	{
		BulletClass* pBullet = *it;
		if (pBullet == nullptr || IsDeadOrInvisible(pBullet))
		{
			it = _decoys.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void DecoyMissile::AddDecoy(BulletClass* pDecoy, CoordStruct launchPos, int life)
{
	MissileTrajectory* status = nullptr;
	if (TryGetScript<BulletExt>(pDecoy, status))
	{
		status->IsDecoy = true;
		status->LaunchPos = launchPos;
		status->LifeTimer.Start(life);

		_decoys.push_back(pDecoy);
	}
}

BulletClass* DecoyMissile::RandomDecoy()
{
	BulletClass* pDecoy = nullptr;
	int count = _decoys.size();
	if (count > 0)
	{
		auto it = _decoys.begin();
		int ans = Random::RandomRanged(0, count - 1);
		if (ans > 0)
		{
			std::advance(it, ans);
		}
		if (it != _decoys.end())
		{
			pDecoy = *it;
			_decoys.erase(it);
		}
	}
	return pDecoy;
}

BulletClass* DecoyMissile::CloseEnoughDecoy(CoordStruct pos, double min)
{
	BulletClass* pDecoy = nullptr;
	double distance = min;
	for (auto it = _decoys.begin(); it != _decoys.end(); it++)
	{
		BulletClass* pBullet = *it;
		if (!IsDeadOrInvisible(pBullet))
		{
			double x = pos.DistanceFrom(pBullet->GetCoords());
			if (x < distance)
			{
				distance = x;
				pDecoy = pBullet;
			}
		}
	}
	return pDecoy;
}

void DecoyMissile::Setup()
{
	_data = nullptr;
	if (IsBuilding() || !(IsFly() || IsJumpjet()) || !GetDecoyMissileData()->Enable)
	{
		Disable();
	}
	_fire = GetDecoyMissileData()->AlwaysFire;
}

void DecoyMissile::Awake()
{
	Setup();
	EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &DecoyMissile::OnMissileDelete);
}

void DecoyMissile::Destroy()
{
	EventSystems::General.RemoveHandler(Events::ObjectUnInitEvent, this, &DecoyMissile::OnMissileDelete);
}

void DecoyMissile::ExtChanged()
{
	Setup();
}

void DecoyMissile::OnRemove()
{
	_decoys.clear();
}

void DecoyMissile::OnUpdate()
{
	if (!IsDeadOrInvisible(pTechno) && pTechno->IsInAir())
	{
		CoordStruct location = pTechno->GetCoords();
		ClearDeadDecoy();

		DecoyMissileData* data = GetDecoyMissileData();
		std::string weaponId = data->Weapon;
		if (pTechno->Veterancy.IsElite())
		{
			weaponId = data->EliteWeapon;
		}
		WeaponTypeClass* pWeapon = nullptr;
		int range = 0;
		if (IsNotNone(weaponId))
		{
			pWeapon = WeaponTypeClass::Find(weaponId.c_str());
			range = pWeapon->Range;
		}

		// Fire decoy
		if (_fire)
		{
			if (pWeapon && DropOne(pWeapon))
			{
				FacingClass facing = pTechno->GetRealFacing();

				CoordStruct flhL = data->FLH;
				if (flhL.Y > 0)
				{
					flhL.Y = -flhL.Y;
				}
				CoordStruct flhR = data->FLH;
				if (flhR.Y < 0)
				{
					flhR.Y = -flhR.Y;
				}

				CoordStruct portL = GetFLH(location, flhL, facing.Desired());
				CoordStruct portR = GetFLH(location, flhR, facing.Desired());

				CoordStruct targetFLHL = flhL + CoordStruct{ 0, -range * 2, 0 };
				CoordStruct targetFLHR = flhR + CoordStruct{ 0, range * 2, 0 };
				CoordStruct targetL = GetFLH(location, targetFLHL, facing.Desired());
				CoordStruct targetR = GetFLH(location, targetFLHR, facing.Desired());

				CoordStruct velocity = data->Velocity;
				if (velocity.IsEmpty() && pWeapon->Projectile->Arcing)
				{
					velocity = data->FLH;
				}
				CoordStruct vL = velocity;
				if (vL.Y > 0)
				{
					vL.Y = -vL.Y;
				}
				vL.Z *= 2;
				CoordStruct vR = velocity;
				if (vR.Y < 0)
				{
					vR.Y = -vR.Y;
				}
				vR.Z *= 2;
				CoordStruct velocityL = GetFLH(CoordStruct::Empty, vL, facing.Desired());
				CoordStruct velocityR = GetFLH(CoordStruct::Empty, vR, facing.Desired());
				for (int i = 0; i < 2; i++)
				{
					CoordStruct initTarget = targetL;
					CoordStruct port = portL;
					BulletVelocity v{ (double)velocityL.X, (double)velocityL.Y, (double)velocityL.Z };
					if (i > 0)
					{
						initTarget = targetR;
						port = portR;
						v = BulletVelocity{ (double)velocityR.X, (double)velocityR.Y, (double)velocityR.Z };
					}
					CellClass* pCell = MapClass::Instance->GetCellAt(initTarget);
					BulletClass* pBullet = pWeapon->Projectile->CreateBullet(pCell, pTechno, pWeapon->Damage, pWeapon->Warhead, pWeapon->Speed, pWeapon->Bright);
					pBullet->WeaponType = pWeapon;
					pBullet->MoveTo(port, v);
					AddDecoy(pBullet, port, data->Life);
				}
			}
			// 将来袭导弹目标设定到最近的诱饵上
			for (BulletClass* pTarget : BulletExt::TargetHasDecoyBullets)
			{
				if (!IsDeadOrInvisible(pTarget) && pTarget->Target == pTechno)
				{
					CoordStruct pos = pTarget->GetCoords();
					BulletClass* pDecoy = CloseEnoughDecoy(pos, location.DistanceFrom(pos));
					if (pDecoy && pDecoy->GetCoords().DistanceFrom(pos) <= range * 2)
					{
						pTarget->SetTarget(pDecoy);
					}
				}
			}
		}
		else
		{
			// 检查到有一发朝向自己发射的导弹时，启动热诱弹发射
			for (BulletClass* pTarget : BulletExt::TargetHasDecoyBullets)
			{
				if (!IsDeadOrInvisible(pTarget) && pTarget->Target == pTechno)
				{
					CoordStruct pos = pTarget->GetCoords();
					// 有一颗导弹进入感应范围，开始抛洒
					if (!_fire && location.DistanceFrom(pos) <= range)
					{
						_fire = true;
					}
					// 将来袭导弹目标设定到最近的诱饵上
					BulletClass* pDecoy = CloseEnoughDecoy(pos, location.DistanceFrom(pos));
					if (pDecoy && pDecoy->GetCoords().DistanceFrom(pos) <= range * 2)
					{
						pTarget->SetTarget(pDecoy);
					}
				}
			}

		}
	}
}

