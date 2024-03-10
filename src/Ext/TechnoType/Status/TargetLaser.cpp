#include "../TechnoStatus.h"

#include <Ext/Helper/DrawEx.h>

void TechnoStatus::StartTargetLaser(AbstractClass* pTarget, int weaponRange, TargetLaserData data, CoordStruct flh, bool isOnTurret)
{
	if (!_hasTargetLaser)
	{
		EventSystems::General.AddHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
	}
	_hasTargetLaser = true;
	_targetLaserTarget = pTarget;
	_targetLaserRange = weaponRange + (int)(data.TargetLaserRange * Unsorted::LeptonsPerCell);
	_targetLaserData = data;
	_targetLaserFLH = flh;
	_targetLaserOnTurret = isOnTurret;
	_targetLaserOffset = data.TargetLaserOffset;
}

void TechnoStatus::CloseTargetLaser()
{
	_hasTargetLaser = false;
	_targetLaserTarget = nullptr;
	_targetLaserRange = -1;
	_targetLaserData.Enable = false;
	_targetLaserFLH = CoordStruct::Empty; // 发射的FLH
	_targetLaserOnTurret = true; // 发射位置在炮塔上
	_targetLaserOffset = CoordStruct::Empty;
	_targetLaserShakeTimer.Stop();
	EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnLaserTargetDetach);
	EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::OnGScreenRender);
}

void TechnoStatus::OnGScreenRender(EventSystem* sender, Event e, void* args)
{
	if (!pTechno)
	{
		return;
	}
	if (args && _targetLaserTarget)
	{
		// EndRender
		CoordStruct sourceLocation = GetFLHAbsoluteCoords(pTechno, _targetLaserFLH, _targetLaserOnTurret);
		// 绘制激光
		CoordStruct targetLocation = _targetLaserTarget->GetCoords();
		CoordStruct offset = _targetLaserData.TargetLaserOffset;
		// 偏移和抖动
		if (!_targetLaserData.TargetLaserShake.IsEmpty())
		{
			offset = _targetLaserOffset;
			// 更新偏移值
			if (_targetLaserShakeTimer.Expired())
			{
				int min = _targetLaserData.TargetLaserShake.X;
				int max = _targetLaserData.TargetLaserShake.Y;
				int d = Random::RandomRanged(min, max);
				_targetLaserOffset = _targetLaserData.TargetLaserOffset + CoordStruct{ d, d, d };
				int delay = Random::RandomRanged(0, 15);
				_targetLaserShakeTimer.Start(delay);
			}
		}
		DirStruct facing = Point2Dir(sourceLocation, targetLocation);
		targetLocation = GetFLHAbsoluteCoords(targetLocation, offset, facing);
		ColorStruct color = _targetLaserData.LaserInnerColor;
		if (_targetLaserData.IsHouseColor)
		{
			color = pTechno->Owner->LaserColor;
		}
		RectangleStruct bounds = DSurface::Temp->GetRect();
		bounds.Height -= 34;
		DrawTargetLaser(DSurface::Temp, sourceLocation, targetLocation, color, bounds);
	}
}

void TechnoStatus::OnUpdate_TargetLaser()
{
	// 检测与目标的距离，并关闭激光笔
	if (_hasTargetLaser)
	{
		TechnoClass* pTargetTechno = nullptr;
		if (!_targetLaserTarget
			|| (CastToTechno(_targetLaserTarget, pTargetTechno) && IsDeadOrInvisibleOrCloaked(pTargetTechno))
			|| (_lastTarget == _targetLaserTarget && !pTechno->Target))
		{
			CloseTargetLaser();
			return;
		}
		if (_targetLaserRange >= 0)
		{
			int dist = pTechno->DistanceFrom(_targetLaserTarget);
			if (dist > _targetLaserRange)
			{
				CloseTargetLaser();
			}
		}
	}
}

void TechnoStatus::OnRemove_TargetLaser()
{
	CloseTargetLaser();
}

void TechnoStatus::OnFire_TargetLaser(AbstractClass* pTarget, int weaponIdx)
{
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	WeaponTypeClass* pWeaponType = pWeapon->WeaponType;
	TargetLaserData* data = INI::GetConfig<TargetLaserData>(INI::Rules, pWeaponType->ID)->Data;
	if (data->Enable)
	{
		CoordStruct flh = pTechno->GetFLH(weaponIdx, CoordStruct::Empty);
		StartTargetLaser(pTarget, pWeaponType->Range, *data, pWeapon->FLH);
	}
}

