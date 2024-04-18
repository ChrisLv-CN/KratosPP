#include "MissileHoming.h"

#include <RocketLocomotionClass.h>

#include <Ext/Helper/FLH.h>

MissileHomingData* MissileHoming::GetHomingData()
{
	if (!_homingData)
	{
		_homingData = INI::GetConfig<MissileHomingData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _homingData;
}

void MissileHoming::Setup()
{
	_homingData = nullptr;
	if (IsRocket())
	{
		if (!IsHoming)
		{
			IsHoming = GetHomingData()->Homing;
		}
	}
	else
	{
		Disable();
	}
}

void MissileHoming::Awake()
{
	Setup();
}

void MissileHoming::OnUpdate()
{
	// 调整起飞姿态，面相目标
	if (!_initHomingFlag)
	{
		_initHomingFlag = true;
		if (GetHomingData()->FacingTarget)
		{
			CoordStruct sourcePos = pTechno->GetCoords();
			CoordStruct targetPos;
			dynamic_cast<FootClass*>(pTechno)->Locomotor->Destination(&targetPos);
			DirStruct dir = Point2Dir(sourcePos, targetPos);
			pTechno->PrimaryFacing.SetCurrent(dir);
			pTechno->SecondaryFacing.SetCurrent(dir);
		}
	}
	// 读取目标位置修改飞行目的地
	if (IsHoming && !IsDeadOrInvisible(pTechno))
	{
		// 更新目标所在的位置
		AbstractClass* pTarget = pTechno->Target;
		if (pTarget)
		{
			TechnoClass* pTargetTechno = nullptr;
			if (CastToTechno(pTarget, pTargetTechno))
			{
				// 如果目标消失，导弹会追到最后一个位置然后爆炸
				if (!IsDeadOrInvisibleOrCloaked(pTargetTechno))
				{
					HomingTargetLocation = pTargetTechno->GetCoords();
				}
			}
		}
		if (!HomingTargetLocation.IsEmpty())
		{
			RocketLocomotionClass* pLoco = dynamic_cast<RocketLocomotionClass*>(dynamic_cast<FootClass*>(pTechno)->Locomotor.get());
			if (pLoco && pLoco->MissionState > 2)
			{
				pLoco->MovingDestination = HomingTargetLocation;
			}
		}
	}
}
