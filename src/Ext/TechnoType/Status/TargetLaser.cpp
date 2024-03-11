#include "../TechnoStatus.h"

#include <Ext/Helper/DrawEx.h>

void TechnoStatus::StartTargetLaser(AbstractClass* pTarget, WeaponTypeClass* pWeapon, TargetLaserData data, CoordStruct flh, bool isOnTurret)
{
	if (_targetLasers.empty())
	{
		EventSystems::General.AddHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
	}
	bool found = false;
	for (TargetLaser& laser : _targetLasers)
	{
		if (laser.pWeapon == pWeapon)
		{
			found = true;
			// 更新设置
			laser.Data = data;
			laser.pTarget = pTarget;
			laser.RangeLimit = pWeapon->Range + (int)laser.Data.TargetLaserRange * Unsorted::LeptonsPerCell;
			laser.FLH = flh;
			laser.IsOnTurret = isOnTurret;
			laser.TargetOffset = data.TargetLaserOffset;
			laser.Duration = data.TargetLaserDuration;
			laser.ResetTimer();
		}
	}
	if (!found)
	{
		// 添加一个新的
		TargetLaser laser{};
		laser.pWeapon = pWeapon;
		laser.Data = data;
		laser.pTarget = pTarget;
		laser.RangeLimit = pWeapon->Range + (int)laser.Data.TargetLaserRange * Unsorted::LeptonsPerCell;
		laser.FLH = flh;
		laser.IsOnTurret = isOnTurret;
		laser.TargetOffset = data.TargetLaserOffset;
		laser.Duration = data.TargetLaserDuration;
		laser.ResetTimer();
		_targetLasers.emplace_back(laser);
	}
}

void TechnoStatus::CloseTargetLaser(AbstractClass* pTarget)
{
	for (auto it = _targetLasers.begin(); it != _targetLasers.end();)
	{
		if ((*it).pTarget == pTarget)
		{
			it = _targetLasers.erase(it);
		}
		else
		{
			it++;
		}
	}
	if (_targetLasers.empty())
	{
		EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
	}
}

void TechnoStatus::CloseTargetLaser(WeaponTypeClass* pWeapon)
{
	for (auto it = _targetLasers.begin(); it != _targetLasers.end();)
	{
		if ((*it).pWeapon == pWeapon)
		{
			it = _targetLasers.erase(it);
		}
		else
		{
			it++;
		}
	}
	if (_targetLasers.empty())
	{
		EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
	}
}

void TechnoStatus::OnGScreenRender(EventSystem* sender, Event e, void* args)
{
	if (!pTechno)
	{
		return;
	}
	if (args)
	{
		RectangleStruct bounds = DSurface::Temp->GetRect();
		bounds.Height -= 34;
		// EndRender
		for (TargetLaser& laser : _targetLasers)
		{
			CoordStruct sourceLocation = GetFLHAbsoluteCoords(pTechno, laser.FLH, laser.IsOnTurret);
			// 绘制激光
			CoordStruct targetLocation = laser.pTarget->GetCoords();
			CoordStruct offset = laser.Data.TargetLaserOffset;
			// 偏移和抖动
			if (!laser.Data.TargetLaserShake.IsEmpty())
			{
				offset = laser.TargetOffset;
				// 更新偏移值
				if (laser.ShakeTimer.Expired())
				{
					int min = laser.Data.TargetLaserShake.X;
					int max = laser.Data.TargetLaserShake.Y;
					int d = Random::RandomRanged(min, max);
					laser.TargetOffset = laser.Data.TargetLaserOffset + CoordStruct{ d, d, d };
					int delay = Random::RandomRanged(0, 15);
					laser.ShakeTimer.Start(delay);
				}
			}
			DirStruct facing = Point2Dir(sourceLocation, targetLocation);
			targetLocation = GetFLHAbsoluteCoords(targetLocation, offset, facing);
			ColorStruct color = laser.Data.LaserInnerColor;
			if (laser.Data.IsHouseColor)
			{
				color = pTechno->Owner->LaserColor;
			}
			DrawTargetLaser(DSurface::Temp, sourceLocation, targetLocation, color, bounds, laser.Data.TargetLaserPoint);
		}

	}
}

void TechnoStatus::OnUpdate_TargetLaser()
{
	if (!_targetLasers.empty())
	{
		// 检测与目标的距离，并关闭激光笔
		for (auto it = _targetLasers.begin(); it != _targetLasers.end();)
		{
			TargetLaser laser = *it;
			TechnoClass* pTargetTechno = nullptr;
			if (!laser.pTarget
				|| (laser.Duration >0 && laser.LifeTimer.Expired())
				|| (CastToTechno(laser.pTarget, pTargetTechno) && IsDeadOrInvisibleOrCloaked(pTargetTechno))
				|| (_lastTarget == laser.pTarget && !pTechno->Target)
				|| (laser.RangeLimit >= 0 && (pTechno->DistanceFrom(laser.pTarget) > laser.RangeLimit)))
			{
				it = _targetLasers.erase(it);
			}
			else
			{
				it++;
			}
		}
		if (_targetLasers.empty())
		{
			EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
			EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
		}
	}
}

void TechnoStatus::OnRemove_TargetLaser()
{
	_targetLasers.clear();
	EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
	EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
}

void TechnoStatus::OnFire_TargetLaser(AbstractClass* pTarget, int weaponIdx)
{
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	WeaponTypeClass* pWeaponType = pWeapon->WeaponType;
	TargetLaserData* data = INI::GetConfig<TargetLaserData>(INI::Rules, pWeaponType->ID)->Data;
	if (data->Enable)
	{
		if (!data->BreakTargetLaser)
		{
			StartTargetLaser(pTarget, pWeaponType, *data, pWeapon->FLH);
		}
		else
		{
			CloseTargetLaser(pTarget);
		}
	}
}

