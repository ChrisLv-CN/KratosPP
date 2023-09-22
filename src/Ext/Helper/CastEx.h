#pragma once

#include <GeneralStructures.h>
#include <TacticalClass.h>

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
