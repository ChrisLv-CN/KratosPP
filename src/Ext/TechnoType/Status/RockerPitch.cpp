#include "../TechnoStatus.h"

#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

void TechnoStatus::RockerPitch(WeaponTypeClass* pWeapon)
{
	WeaponTypeExt::TypeData* weaponData = nullptr;
	if (TryGetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon, weaponData) && weaponData->RockerPitch > 0)
	{
		double halfPI = Math::HalfPi;
		// 获取转角
		double theta = 0;
		if (pTechno->HasTurret())
		{
			double turretRad = pTechno->GetRealFacing().Current().GetRadian() - halfPI;
			double bodyRad = pTechno->PrimaryFacing.Current().GetRadian() - halfPI;
			Matrix3D matrix3D;
			matrix3D.RotateZ((float)turretRad);
			matrix3D.RotateZ((float)-bodyRad);
			theta = matrix3D.GetZRotation();
		}
		// 抬起的角度
		double gamma = weaponData->RockerPitch;
		// 符号
		int lrSide = 1;
		int fbSide = 1;
		if (theta < 0)
		{
			lrSide *= -1;
		}
		if (theta >= halfPI || theta <= -halfPI)
		{
			fbSide *= -1;
		}
		// 抬起的角度
		double pitch = gamma;
		double roll = 0.0;
		if (theta != 0)
		{
			if (Math::sin(halfPI - theta) == 0)
			{
				pitch = 0.0;
				roll = gamma * lrSide;
			}
			else
			{
				// 以底盘朝向为y轴做相对三维坐标系
				// 在三维坐标系中对于地面γ度，对x轴π/2-θ做一个长度为1线段 L
				// 这条线段在地面投影的长度为
				double l = Math::cos(gamma);
				// L在y轴上的投影长度为
				double y = l / Math::sin(halfPI - theta);
				// L在x轴上的投影长度为
				// double x = l / Math::cos(halfPI - abs(theta));
				// L在z轴上的投影长度为
				double z = Math::sin(gamma);
				// L在yz面上的投影长度为
				double lyz = Math::sqrt(pow(y, 2) + pow(z, 2));
				// L在xz面上的投影长度为
				// double lxz = Math::sqrt(pow(x, 2) + pow(z, 2));

				pitch = Math::acos(abs(y) / lyz) * fbSide;
				// roll = Math::acos(x / lxz) * lrSide;
				roll = (gamma - abs(pitch)) * lrSide;
			}
		}
		pTechno->RockingForwardsPerFrame = -(float)pitch;
		pTechno->RockingSidewaysPerFrame = (float)roll;
	}
}


void TechnoStatus::OnFire_RockerPitch(AbstractClass* pTarget, int weaponIdx)
{
	if (_isVoxel)
	{
		if (WeaponTypeClass* pWeapon = pTechno->GetWeapon(weaponIdx)->WeaponType)
		{
			RockerPitch(pWeapon);
		}
	}
}
