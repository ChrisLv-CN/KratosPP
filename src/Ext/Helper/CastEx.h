#pragma once

#include <YRPP.h>

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
static BulletVelocity ToVelocity(Vector3D<T>& vector)
{
	return ToVector3D<double>(vector);
}
