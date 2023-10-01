#include "../TechnoStatus.h"
#include <Ext/CommonStatus.h>

#include <RocketLocomotionClass.h>

MissileHomingData* TechnoStatus::GetHomingData()
{
	if (!_homingData)
	{
		_homingData = INI::GetConfig<MissileHomingData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _homingData;
}

void TechnoStatus::OnUpdate_MissileHoming()
{
	if (IsRocket())
	{
		if (!_initHomingFlag)
		{
			_initHomingFlag = true;
			if (GetHomingData()->FacingTarget)
			{
				CoordStruct sourcePos = pTechno->GetCoords();
				CoordStruct targetPos;
				static_cast<FootClass*>(pTechno)->Locomotor->Destination(&targetPos);
				DirStruct dir = Point2Dir(sourcePos, targetPos);
				pTechno->PrimaryFacing.SetCurrent(dir);
				pTechno->SecondaryFacing.SetCurrent(dir);
			}
		}
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
				RocketLocomotionClass* pLoco = static_cast<RocketLocomotionClass*>(static_cast<FootClass*>(pTechno)->Locomotor.get());
				if (pLoco->MissionState > 2)
				{
					pLoco->MovingDestination = HomingTargetLocation;
				}
			}
		}
	}
}
