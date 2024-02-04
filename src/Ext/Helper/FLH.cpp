#include "FLH.h"
#include "CastEx.h"
#include "MathEx.h"

#include <LocomotionClass.h>
#include <JumpjetLocomotionClass.h>

#include <Common/INI/INI.h>

// ----------------
// FLHHelper
// ----------------
#pragma region FLH
CoordStruct GetFLH(CoordStruct source, CoordStruct flh, DirStruct dir, bool flip)
{
	CoordStruct res = source;
	if (flh != CoordStruct::Empty)
	{
		double radians = dir.GetRadian();
		double rF = flh.X;
		int xF = static_cast<int>(rF * Math::cos(-radians));
		int yF = static_cast<int>(rF * Math::sin(-radians));
		CoordStruct offsetF{ xF, yF, 0 };

		double rL = flip ? flh.Y : -flh.Y;
		int xL = static_cast<int>(rL * Math::sin(radians));
		int yL = static_cast<int>(rL * Math::cos(radians));
		CoordStruct offsetL{ xL, yL, 0 };

		CoordStruct offsetH{ 0, 0, flh.Z };

		res = source + offsetF + offsetL + offsetH;
	}
	return res;
}


// Step 1: get body transform matrix
Matrix3D GetMatrix3D(TechnoClass* pTechno, VoxelIndexKey* pKey, bool isShadow)
{
	Matrix3D mtx;

	if ((pTechno->AbstractFlags & AbstractFlags::Foot) && ((FootClass*)pTechno)->Locomotor)
	{
		mtx = isShadow ? ((FootClass*)pTechno)->Locomotor->Shadow_Matrix(pKey) : ((FootClass*)pTechno)->Locomotor->Draw_Matrix(pKey);
	}
	else // no locomotor means no rotation or transform of any kind (f.ex. buildings) - Kerbiter
	{
		mtx.MakeIdentity();
	}
	return mtx;
}

// Step 2: move to turretOffset
Matrix3D MoveMatrix3DOnTurret(Matrix3D& mtx, CoordStruct turretOffset, double factor)
{
	float x = static_cast<float>(turretOffset.X * factor);
	float y = static_cast<float>(turretOffset.Y * factor);
	float z = static_cast<float>(turretOffset.Z * factor);

	mtx.Translate(x, y, z);
	return mtx;
}

// Step 3: rotation to turretDir
Matrix3D RotateMatrix3DOnTurret(Matrix3D& mtx, double turretRad, bool isBuilding)
{
	/*
	double turretRad = pTechno->TurretFacing().GetRadian();
	double bodyRad = pTechno->PrimaryFacing.Current().GetRadian();
	float angle = (float)(turretRad - bodyRad);

	mtx.RotateZ(angle);
	*/

	// rotate to turret's angle
	if (isBuilding)
	{
		mtx.RotateZ(static_cast<float>(turretRad));
	}
	else
	{
		mtx.RotateZ(-mtx.GetZRotation());
		mtx.RotateZ(static_cast<float>(turretRad));
	}
	return mtx;
}

// Step 4: apply FLH offset
Vector3D<float> GetFLHOffset(Matrix3D& mtx, CoordStruct flh)
{
	mtx.Translate(ToVector3D<float>(flh));
	// Vector3D<float> res = Matrix3D::MatrixMultiply(mtx, Vector3D<float>::Empty);
	Vector3D<float> res = mtx * Vector3D<float>::Empty;
	// Resulting FLH is mirrored along X axis, so we mirror it back - Kerbiter
	res.Y *= -1;
	return res;
}

CoordStruct GetFLHAbsoluteOffset(CoordStruct flh, DirStruct dir, CoordStruct turretOffset)
{
	CoordStruct res = CoordStruct::Empty;
	if (flh != CoordStruct::Empty)
	{
		Matrix3D mtx;
		mtx.MakeIdentity();
		mtx.Translate(ToVector3D<float>(turretOffset));
		mtx.RotateZ((float)dir.GetRadian());
		Vector3D<float> offset = GetFLHOffset(mtx, flh);

		res.X = std::lround(offset.X);
		res.Y = std::lround(offset.Y);
		res.Z = std::lround(offset.Z);
	}
	return res;
}

CoordStruct GetFLHAbsoluteCoords(CoordStruct source, CoordStruct flh, DirStruct dir, CoordStruct turretOffset)
{
	CoordStruct res = source;
	if (flh != CoordStruct::Empty)
	{
		CoordStruct offset = GetFLHAbsoluteOffset(flh, dir, turretOffset);
		res += offset;
	}
	return res;
}

// reversed from 6F3D60
CoordStruct GetFLHAbsoluteCoords(TechnoClass* pTechno, CoordStruct flh, bool isOnTurret, int flipY, CoordStruct turretOffset)
{
	CoordStruct source = pTechno->GetCoords();
	if (pTechno->WhatAmI() == AbstractType::Building)
	{
		// Building can't use Matrix3D get FLH
		CoordStruct tempFLH = flh;
		tempFLH.Y += flipY;
		DirStruct dir = pTechno->PrimaryFacing.Current();
		return GetFLHAbsoluteCoords(source, tempFLH, dir, turretOffset);
	}
	else
	{
		if (flh != CoordStruct::Empty || turretOffset != CoordStruct::Empty)
		{
			// step 1
			Matrix3D mtx = GetMatrix3D(pTechno);
			// step 2
			if (isOnTurret)
			{
				// read the ini config data
				CoordStruct offset = INI::GetSection(INI::Art, pTechno->GetType()->ID)->Get("TurrentOffset", CoordStruct::Empty);
				offset += turretOffset;
				MoveMatrix3DOnTurret(mtx, offset);

				// step 3
				if (pTechno->HasTurret())
				{
					DirStruct turretDir = pTechno->SecondaryFacing.Current();
					double turretRad = turretDir.GetRadian();
					RotateMatrix3DOnTurret(mtx, turretRad, false);
				}
			}
			// step 4
			CoordStruct tempFLH = flh;
			tempFLH.Y *= flipY;
			Vector3D<float> offset = GetFLHOffset(mtx, tempFLH);
			source.X += std::lround(offset.X);
			source.Y += std::lround(offset.Y);
			source.Z += std::lround(offset.Z);
		}
	}
	return source;
}
#pragma endregion

#pragma region RelativeLocation
DirStruct GetRelativeDir(TechnoClass* pMaster, int dir, bool isOnTurret)
{
	// turn offset
	DirStruct targetDir = DirNormalized(dir, 16);
	FootClass* pFoot = nullptr;
	if (CastToFoot(pMaster, pFoot))
	{
		double targetRad = targetDir.GetRadian();
		DirStruct sourceDir = pMaster->PrimaryFacing.Current();
		GUID locoId = pMaster->GetTechnoType()->Locomotor;
		if (locoId == LocomotionClass::CLSIDs::Jumpjet)
		{
			sourceDir = static_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get())->LocomotionFacing.Current();
		}
		if (isOnTurret || pFoot->WhatAmI() == AbstractType::Aircraft) // WWSB Aircraft is a turret!!!
		{
			sourceDir = pMaster->GetRealFacing().Current();
		}
		double sourceRad = sourceDir.GetRadian();
		float angle = static_cast<float>(sourceRad - targetRad);
		targetDir = Radians2Dir(angle);
	}
	return targetDir;
}

LocationMark GetRelativeLocation(ObjectClass* pOwner, OffsetData data, CoordStruct offset)
{
	if (offset.IsEmpty())
	{
		offset = data.Offset;
	}
	CoordStruct sourcePos = pOwner->Location;

	CoordStruct targetPos = sourcePos;
	DirStruct targetDir{};
	if (data.IsOnWorld)
	{
		// 绑定世界坐标，朝向固定北向
		targetPos = GetFLHAbsoluteCoords(sourcePos, offset, targetDir);
	}
	else
	{
		// 绑定单体坐标
		TechnoClass* pTechno = nullptr;
		BulletClass* pBullet = nullptr;
		if (CastToTechno(pOwner, pTechno))
		{
			targetDir = GetRelativeDir(pTechno, data.Direction, data.IsOnTurret);
			targetPos = GetFLHAbsoluteCoords(pTechno, offset, data.IsOnTurret);
		}
		else if (CastToBullet(pOwner, pBullet))
		{
			// 增加抛射体偏移值取下一帧所在实际位置
			sourcePos += ToCoordStruct(pBullet->Velocity);
			// 获取面向
			targetDir = Point2Dir(sourcePos, pBullet->TargetCoords);
			targetPos = GetFLHAbsoluteCoords(sourcePos, offset, targetDir);
		}
	}
	return LocationMark{ targetPos, targetDir };
}

DirStruct GetRelativeDirection(ObjectClass* pOwner, int dir, bool isOnTurret, bool isOnWorld)
{
	DirStruct targetDir{};
	if (!isOnWorld)
	{
		// 绑定单体坐标
		TechnoClass* pTechno = nullptr;
		BulletClass* pBullet = nullptr;
		if (CastToTechno(pOwner, pTechno))
		{
			targetDir = GetRelativeDir(pTechno, dir, isOnTurret);
		}
		else if (CastToBullet(pOwner, pBullet))
		{
			// 增加抛射体偏移值取下一帧所在实际位置
			CoordStruct sourcePos = pBullet->Location;
			sourcePos += ToCoordStruct(pBullet->Velocity);
			// 获取面向
			targetDir = Point2Dir(sourcePos, pBullet->TargetCoords);
		}
	}
	return targetDir;
}
#pragma endregion

#pragma region ForwardFLH
/// @brief 获取向量上指定距离的坐标
/// @param sourceV 起始位置
/// @param targetV 结束位置
/// @param speed 速度
/// @param dist 距离
/// @return CoordStruct
CoordStruct GetForwardCoords(Vector3D<double> sourceV, Vector3D<double> targetV, double speed, double dist)
{
	if (dist <= 0)
	{
		dist = targetV.DistanceFrom(sourceV);
	}
	// 计算下一个坐标
	double d = speed / dist;
	double absX = abs(sourceV.X - targetV.X) * d;
	double x = sourceV.X;
	if (sourceV.X < targetV.X)
	{
		// Xa < Xb => Xa < Xc
		// Xc - Xa = absX
		x = absX + sourceV.X;
	}
	else if (sourceV.X > targetV.X)
	{
		// Xa > Xb => Xa > Xc
		// Xa - Xc = absX
		x = sourceV.X - absX;
	}
	double absY = abs(sourceV.Y - targetV.Y) * d;
	double y = sourceV.Y;
	if (sourceV.Y < targetV.Y)
	{
		y = absY + sourceV.Y;
	}
	else if (sourceV.Y > targetV.Y)
	{
		y = sourceV.Y - absY;
	}
	double absZ = abs(sourceV.Z - targetV.Z) * d;
	double z = sourceV.Z;
	if (sourceV.Z < targetV.Z)
	{
		z = absZ + sourceV.Z;
	}
	else if (sourceV.Z > targetV.Z)
	{
		z = sourceV.Z - absZ;
	}
	Vector3D<double> v{ x, y, z };
	return ToCoordStruct(v);
}

CoordStruct GetForwardCoords(CoordStruct sourcePos, CoordStruct targetPos, double speed, double dist)
{
	return GetForwardCoords(ToVelocity(sourcePos), ToVelocity(targetPos), speed, dist);
}
#pragma endregion

#pragma region Random offset
CoordStruct GetRandomOffset(int min, int max)
{
	double r = Random::RandomRanged(min, max);
	if (r > 0)
	{
		double theta = Random::RandomDouble() * Math::TwoPi;
		CoordStruct offset{ static_cast<int>(r * Math::cos(theta)), static_cast<int>(r * Math::sin(theta)), 0 };
		return offset;
	}
	return CoordStruct::Empty;
}

CoordStruct GetRandomOffset(double maxSpread, double minSpread)
{
	int min = static_cast<int>((minSpread <= 0 ? 0 : minSpread) * 256);
	int max = static_cast<int>((maxSpread > 0 ? maxSpread : 1) * 256);
	return GetRandomOffset(min, max);
}
#pragma endregion

#pragma region Dir
DirStruct DirNormalized(int index, int facing)
{
	double r = Math::deg2rad((-360 / facing * index));
	DirStruct dir;
	dir.SetValue((short)(r / BINARY_ANGLE_MAGIC));
	return dir;
}

int Dir2FacingIndex(DirStruct dir, int facing)
{
	size_t bits = (size_t)ceil0(Math::sqrt(facing));
	double face = dir.GetValue(bits);
	double x = (face / (1 << static_cast<int>(bits))) * facing;
	int index = static_cast<int>(ceil0(x));
	return index;
}

int Dir2FrameIndex(DirStruct dir, int facing)
{
	int index = Dir2FacingIndex(dir, facing);
	index = static_cast<int>(facing / 8) + index;
	if (index >= facing)
	{
		index -= facing;
	}
	return index;
}

DirStruct Radians2Dir(double radians)
{
	short d = (short)(radians / BINARY_ANGLE_MAGIC);
	return DirStruct{ d };
}

DirStruct Point2Dir(CoordStruct source, CoordStruct target)
{
	double radians = Math::atan2(source.Y - target.Y, target.X - source.X);
	radians -= Math::deg2rad(90);
	return Radians2Dir(radians);
}
#pragma endregion
