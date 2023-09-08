#pragma once

#include <cmath>
#include <string>

#include <YRPP.h>


//--------------------------------------------------------------------------
// symmetric round up
// Bias: away from zero
template <typename FloatType>
FloatType ceil0(const FloatType& value)
{
	FloatType result = std::ceil(std::fabs(value));
	return (value < 0.0) ? -result : result;
}

// ----------------
// FLHHelper
// ----------------
#pragma region FLH
CoordStruct GetFLH(CoordStruct source, CoordStruct flh, DirStruct dir, bool flip = false);

// Step 1: get body transform matrix
Matrix3D GetMatrix3D(TechnoClass* pTechno, VoxelIndexKey* pKey = nullptr, bool isShadow = false);

// Step 2: move to turretOffset
Matrix3D MoveMatrix3DOnTurret(Matrix3D& mtx, CoordStruct turretOffset, double factor = 1.0);

// Step 3: rotation to turretDir
Matrix3D RotateMatrix3DOnTurret(Matrix3D& mtx, double turretRad, bool isBuilding);

// Step 4: apply FLH offset
Vector3D<float> GetFLHOffset(Matrix3D& mtx, CoordStruct flh);

CoordStruct GetFLHAbsoluteOffset(CoordStruct flh, DirStruct dir, CoordStruct turretOffset);

CoordStruct GetFLHAbsoluteCoords(CoordStruct source, CoordStruct flh, DirStruct dir, CoordStruct turretOffset = CoordStruct::Empty);

// reversed from 6F3D60
CoordStruct GetFLHAbsoluteCoords(TechnoClass* pTechno, CoordStruct flh, bool isOnTurret, int flipY, CoordStruct turretOffset);
#pragma endregion

#pragma region Random offset

CoordStruct RandomOffset(int min, int max);
/// @brief 最小1格范围内的随机坐标偏移
/// @param maxSpread 最大半径
/// @param minSpread 最小半径
/// @return offset
CoordStruct RandomOffset(double maxSpread, double minSpread = 0);

#pragma endregion

#pragma region Dir
const double BINARY_ANGLE_MAGIC = -(360.0 / (65535 - 1)) * (Math::Pi / 180);

/// @brief 获得一个指定朝向的DirStruct
/// @param index 序号
/// @param facing 面数
/// @return dir
DirStruct DirNormalized(int index, int facing);

/// @brief 8的倍数分割面向，顺时针，0为↗，Dir的0方向
/// @param dir 朝向
/// @param facing 面数
/// @return index
int Dir2FacingIndex(DirStruct dir, int facing);

/// @brief 0的方向是游戏中的北方，是↗，SHP素材0帧是朝向0点，是↑
/// @param dir 朝向
/// @param facing 面数
/// @return index
int Dir2FrameIndex(DirStruct dir, int facing);

DirType ToDirType(DirStruct dir);

DirStruct Radians2Dir(double radians);
DirStruct Point2Dir(CoordStruct source, CoordStruct target);
#pragma endregion
