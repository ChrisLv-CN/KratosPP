#pragma once

#include <bitset>

#include <GeneralDefinitions.h>
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

static CoordStruct ToCoords(Point2D point)
{
	return TacticalClass::Instance->ClientToCoords(point);
}

static Point2D ToClientPos(CoordStruct& coords)
{
	return TacticalClass::Instance->CoordsToClient(coords);
}

static Point2D CoordsToScreen(CoordStruct coords)
{
	Point2D pos{ 0, 0 };
	TacticalClass::Instance->CoordsToScreen(&pos, &coords);
	return pos;
}

static bool CastToBullet(AbstractClass* pTarget, BulletClass*& pBullet)
{
	pBullet = dynamic_cast<BulletClass*>(pTarget);
	return pBullet != nullptr;
	/*
	if (pTarget)
	{
		switch (pTarget->WhatAmI())
		{
		case AbstractType::Bullet:
			pBullet = dynamic_cast<BulletClass*>(pTarget);
			return pBullet != nullptr;
		default:
			return false;
		}
	}
	return false;
	*/
}

static bool CastToBullet(ObjectClass* pObject, BulletClass*& pBullet)
{
	return CastToBullet(dynamic_cast<AbstractClass*>(pObject), pBullet);
}

static bool CastToTechno(AbstractClass* pTarget, TechnoClass*& pTechno)
{
	pTechno = dynamic_cast<TechnoClass*>(pTarget);
	return pTechno != nullptr;
	/*
	if (pTarget)
	{
		switch (pTarget->WhatAmI())
		{
		case AbstractType::Building:
		case AbstractType::Unit:
		case AbstractType::Infantry:
		case AbstractType::Aircraft:
			pTechno = dynamic_cast<TechnoClass*>(pTarget);
			return pTechno != nullptr;
		default:
			return false;
		}
	}
	return false;
	*/
}
static bool CastToTechno(ObjectClass* pObject, TechnoClass*& pTechno)
{
	return CastToTechno(dynamic_cast<AbstractClass*>(pObject), pTechno);
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

static ColorStruct ToColorAdd(ColorStruct color)
{
	BYTE B = color.B >> 3;
	BYTE G = color.G >> 2;
	BYTE R = color.R >> 3;
	return ColorStruct{ R, G, B };
}

static unsigned int Add2RGB565(ColorStruct colorAdd)
{
	// 转2进制字符串
	std::bitset<5> R2(colorAdd.R);
	std::bitset<6> G2(colorAdd.G);
	std::bitset<5> B2(colorAdd.B);
	// 拼接字符串
	std::string C = R2.to_string();
	C += G2.to_string();
	C += B2.to_string();
	std::bitset<16> C2(C);
	return static_cast<unsigned int>(C2.to_ulong());
}

static unsigned int GetBright(unsigned int bright, float multi)
{
	double b = bright;
	if (multi != 1.0f)
	{
		b *= multi;
		if (b < 0)
		{
			b = 0;
		}
		else if (b > 2000)
		{
			b = 2000;
		}
	}
	return static_cast<unsigned int>(b);
}
