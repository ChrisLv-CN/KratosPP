#include "JumpjetFacing.h"

#include <JumpjetLocomotionClass.h>

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>

JumpjetFacingData* JumpjetFacing::GetJJFacingData()
{
	if (!_jjFacingData)
	{
		_jjFacingData = INI::GetConfig<JumpjetFacingData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _jjFacingData;
}

void JumpjetFacing::SetupJJFacing()
{
	_jjFacingData = nullptr;
	if (!GetJJFacingData()->Enable || !IsJumpjet())
	{
		Disable();
	}
}

void JumpjetFacing::Awake()
{
	SetupJJFacing();
}

void JumpjetFacing::ExtChanged()
{
	SetupJJFacing();
}

void JumpjetFacing::OnUpdate()
{
	if (!IsDeadOrInvisible(pTechno) && pTechno->IsInAir())
	{
		if (_JJNeedTurn)
		{
			FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
			if (pFoot->GetCurrentSpeed() == 0)
			{
				// Turning
				_JJNeedTurn = false;
				pFoot->StopMoving();
				JumpjetLocomotionClass* jjLoco = dynamic_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get());
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
			}
		}
		else
		{
			// Cancel
			_JJNeedTurn = false;
		}
	}
}
