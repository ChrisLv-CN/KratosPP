#pragma once

#include <GeneralStructures.h>
#include <TacticalClass.h>
#include <ObjectClass.h>
#include <BulletClass.h>
#include <TechnoClass.h>
#include <FootClass.h>

template<typename T, typename T2>
static Vector3D<T> ToVector3D(Vector3D<T2>& vector)
{
	return { static_cast<T>(vector.X), static_cast<T>(vector.Y), static_cast<T>(vector.Z) };
}

template<typename T>
static CoordStruct ToCoordStruct(Vector3D<T>& vector)
{
	return ToVector3D<int>(vector);
}

template<typename T>
static Vector3D<double> ToVelocity(Vector3D<T>& vector)
{
	return ToVector3D<double>(vector);
}

static Point2D ToClientPos(CoordStruct& coords)
{
	Point2D pos{ 0, 0 };
	TacticalClass::Instance->CoordsToClient(coords, &pos);
	return pos;
}

static Point2D CoordsToScreen(CoordStruct coords)
{
	Point2D pos{ 0, 0 };
	TacticalClass::Instance->CoordsToScreen(&pos, &coords);
	return pos;
}

static bool CastToBullet(ObjectClass* pObject, BulletClass*& pBullet)
{
	switch (pObject->What_Am_I())
	{
		case AbstractType::Bullet:
			pBullet = dynamic_cast<BulletClass*>(pObject);
			return pBullet != nullptr;
		default:
			return false;
	}
}

static bool CastToTechno(ObjectClass* pObject, TechnoClass*& pTechno)
{
	switch (pObject->What_Am_I())
	{
		case AbstractType::Building:
		case AbstractType::Unit:
		case AbstractType::Infantry:
		case AbstractType::Aircraft:
			pTechno = dynamic_cast<TechnoClass*>(pObject);
			return pTechno != nullptr;
		default:
			return false;
	}
}

static bool CastToFoot(TechnoClass* pTechno, FootClass*& pFoot)
{
	if (pTechno->AbstractFlags & AbstractFlags::Foot)
	{
		pFoot = dynamic_cast<FootClass*>(pTechno);
		return pFoot != nullptr;
	}
	return false;
}

