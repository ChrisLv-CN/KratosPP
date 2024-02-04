#include "../TechnoStatus.h"

#include <JumpjetLocomotionClass.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/FLH.h>

JumpjetFacingData* TechnoStatus::GetJJFacingData()
{
	if (!_jjFacingData)
	{
		_jjFacingData = INI::GetConfig<JumpjetFacingData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _jjFacingData;
}

void TechnoStatus::OnUpdate_JJFacing()
{
	if (!IsDeadOrInvisible(pTechno) && IsJumpjet() && GetJJFacingData()->Enable && pTechno->IsInAir())
	{
		if (_JJNeedTurn)
		{
			FootClass* pFoot = static_cast<FootClass*>(pTechno);
			if (pFoot->GetCurrentSpeed() == 0)
			{
				// Turning
				_JJNeedTurn = false;
				pFoot->StopMoving();
				JumpjetLocomotionClass* jjLoco = static_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get());
				jjLoco->LocomotionFacing.SetDesired(_JJTurnTo);
			}
			else
			{
				// Cancel
				_JJNeedTurn = false;
			}
		}
		else if (AbstractClass* pTarget = pTechno->Target)
		{
			bool canFire = false;
			int weaponIdx = pTechno->SelectWeapon(pTarget);
			FireError fireError = pTechno->GetFireError(pTarget, weaponIdx, true);
			switch (fireError)
			{
			case FireError::ILLEGAL:
			case FireError::CANT:
			case FireError::MOVING:
			case FireError::RANGE:
				break;
			default:
				canFire = pTechno->IsCloseEnough(pTarget, weaponIdx);
				break;
			}
			if (canFire)
			{
				CoordStruct sourcePos = pTechno->GetCoords();
				CoordStruct targetPos = pTarget->GetCoords();
				DirStruct toDir = Point2Dir(sourcePos, targetPos);
				// 计算与当前朝向的误差，是否需要进行旋转
				DirStruct selfDir = pTechno->PrimaryFacing.Current();
				if (toDir.GetRadian() != selfDir.GetRadian())
				{
					// TurnTo(toDir);
					_JJNeedTurn = true;
					_JJTurnTo = toDir;
				}
				else
				{
					// Cancel
					_JJNeedTurn = false;
				}
				/*
				int facing = GetJJFacingData()->Facing;
				int toIndex = Dir2FacingIndex(toDir, facing);
				int selfIndex = Dir2FacingIndex(selfDir, facing);
				if (selfIndex != toIndex)
				{
					// TurnTo(toDir);
					_JJNeedTurn = true;
					_JJTurnTo = toDir;
				}
				else
				{
					// Cancel
					_JJNeedTurn = false;
				}*/
			}
		}
		else
		{
			// Cancel
			_JJNeedTurn = false;
		}
	}
}
