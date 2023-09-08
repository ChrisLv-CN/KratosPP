#pragma once

#include <GeneralStructures.h>

template<typename T, typename T2>
Vector3D<T> ToVector3D(Vector3D<T2>& vector)
{
	return { static_cast<T>(vector.X),static_cast<T>(vector.Y),static_cast<T>(vector.Z) };
}

template<typename T>
CoordStruct ToCoordStruct(Vector3D<T>& vector)
{
	return ToVector3D<int>(vector);
}
