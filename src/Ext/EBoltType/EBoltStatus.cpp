#include "EBoltStatus.h"

#include <Ext/Helper/DrawEx.h>
#include <Ext/Helper/FLH.h>

void EBoltStatus::AttachTo(TechnoClass* pTechno, CoordStruct flh, bool isOnTurret, AbstractClass* pTarget)
{
	_owner = pTechno;
	_flh = flh;
	_isOnTurret = isOnTurret;
	// if (pTarget && pTarget->WhatAmI() == AbstractType::Overlay)
	// {
	// 	// Debug::Log("Target %d is FakeTarget\n", pTarget);
	// 	CoordStruct p1 = GetFLHAbsoluteCoords(pTechno, CoordStruct{ 1, 0, 0 }, isOnTurret);
	// 	CoordStruct p2 = pTarget->GetCoords();
	// 	int z = p2.Z - p1.Z;
	// 	double r = Math::atan2(p1.Y - p2.Y, p2.X - p1.X);

	// 	// r -= pTechno->SecondaryFacing.Current().GetRadian();
	// 	DirStruct targetDir = Radians2Dir(r);


	// 	LaserHelper::GreenCrosshair(pTechno->GetCoords(), 128, 1, 75);
	// 	LaserHelper::RedCrosshair(p1, 128, 1, 75);

	// 	CoordStruct tp = GetFLHAbsoluteCoords(p1, CoordStruct{ 2048, 0, 0 }, targetDir);
	// 	LaserHelper::RedLine(p1, tp, 1, 75);

	// 	p1.Z = 0;
	// 	p2.Z = 0;
	// 	double dist = p1.DistanceFrom(p2);
	// 	if (!isnan(dist) && !isinf(dist))
	// 	{
	// 		_targetFLH = GetFLHAbsoluteCoords(CoordStruct::Empty, CoordStruct{ (int)dist, 0, 0 }, targetDir);
	// 		_targetFLH.Z = z;
	// 		Debug::Log("Dist = %d, TargetFLH { %d, %d, %d }\n", (int)dist, _targetFLH.X, _targetFLH.Y, _targetFLH.Z);
	// 	}
	// }
	EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &EBoltStatus::OnTechnoDelete);
}

void EBoltStatus::OnTechnoDelete(EventSystem* sender, Event e, void* args)
{
	if (args == _owner)
	{
		_owner = nullptr;
	}
}

void EBoltStatus::OnDraw()
{
	if (_owner)
	{
		pBolt->Point1 = GetFLHAbsoluteCoords(_owner, _flh, _isOnTurret);
		if (!_targetFLH.IsEmpty())
		{
			pBolt->Point2 = GetFLHAbsoluteCoords(_owner, _targetFLH, _isOnTurret);
		}
	}
}

void EBoltStatus::Destroy()
{
	EventSystems::General.RemoveHandler(Events::ObjectUnInitEvent, this, &EBoltStatus::OnTechnoDelete);
}

