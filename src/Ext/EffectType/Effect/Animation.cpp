#include "Animation.h"

#include <AnimTypeClass.h>

#include <Extension/AnimExt.h>

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/AnimType/AnimStatus.h>

EFFECT_SCRIPT_CPP(Animation);

void Animation::UpdateLocationOffset(CoordStruct offset)
{
	AnimStatus* animStatus = nullptr;
	if (pAnim && TryGetStatus<AnimExt, AnimStatus>(pAnim, animStatus))
	{
		animStatus->Offset = offset;
	}

}

void Animation::CreateIdleAnim(bool force, CoordStruct location)
{
	if (pAnim)
	{
		KillIdleAnim();
	}
	if (!force && (!Data.IdleAnim.Enable || IsDeadOrInvisible(pObject) || (Data.IdleAnim.RemoveInCloak && ownerIsCloak)))
	{
		return;
	}
	// 创建动画
	AnimTypeClass* pAnimType = AnimTypeClass::Find(Data.ActiveAnim.Type.c_str());
	if (pAnimType)
	{
		OffsetData offsetData = Data.ActiveAnim.Offset;
		if (location.IsEmpty())
		{
			location = GetRelativeLocation(pObject, offsetData).Location;
		}
		AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
		pAnim->RemainingIterations = 0xFF; // Loops
		if (pBullet)
		{
			SetAnimOwner(pAnim, pBullet);
			SetAnimCreater(pAnim, pBullet);
		}
		else if (pTechno)
		{
			SetAnimOwner(pAnim, pTechno);
			SetAnimCreater(pAnim, pTechno);
		}
		ShowAnim(pAnim, Data.IdleAnim.Visibility);
		// 记录动画的渲染参数
		animFlags = pAnim->AnimFlags;
		// 设置动画的附着对象，由动画自身去位移
		AnimStatus* status = GetStatus<AnimExt, AnimStatus>(pAnim);
		status->pAttachOwner = pObject;
		status->AttachToObject(pObject, offsetData);
	}
}

void Animation::KillIdleAnim()
{
	if (pAnim)
	{
		// 不将动画附着于单位上，动画就不会自行注销，需要手动注销
		pAnim->TimeToDie = true;
		pAnim->UnInit(); // 包含了SetOwnerObject(0) 0x4255B0
		pAnim = nullptr;
	}
}

void Animation::Enable() {
	// 激活动画
	if (Data.ActiveAnim.Enable)
	{
		AnimTypeClass* pAnimType = AnimTypeClass::Find(Data.ActiveAnim.Type.c_str());
		if (pAnimType)
		{
			OffsetData offsetData = Data.ActiveAnim.Offset;
			CoordStruct location = GetRelativeLocation(pObject, offsetData).Location;
			AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
			if (pBullet)
			{
				SetAnimOwner(pAnim, pBullet);
				SetAnimCreater(pAnim, pBullet);
			}
			else if (pTechno)
			{
				SetAnimOwner(pAnim, pTechno);
				SetAnimCreater(pAnim, pTechno);
			}
		}
	}
	// 持续动画
	CreateIdleAnim();
}

void Animation::Disable(CoordStruct location)
{
	KillIdleAnim();
	if (Data.DoneAnim.Enable)
	{
		AnimTypeClass* pAnimType = AnimTypeClass::Find(Data.DoneAnim.Type.c_str());
		if (pAnimType)
		{
			if (pObject)
			{
				OffsetData offsetData = Data.DoneAnim.Offset;
				location = GetRelativeLocation(pObject, offsetData).Location;
			}
			AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
			if (!AE->OwnerIsDead())
			{
				if (pBullet)
				{
					SetAnimOwner(pAnim, pBullet);
					SetAnimCreater(pAnim, pBullet);
				}
				else if (pTechno)
				{
					SetAnimOwner(pAnim, pTechno);
					SetAnimCreater(pAnim, pTechno);
				}
			}
		}
	}
}

void Animation::OnPut(CoordStruct* pCoords, DirType faceDir)
{
	CreateIdleAnim(true, *pCoords);
}

void Animation::OnUpdate()
{
	if (pTechno && AE && !AE->OwnerIsDead())
	{
		switch (pTechno->CloakState)
		{
		case CloakState::Uncloaked:
			// 显形状态
			if (ownerIsCloak)
			{
				ownerIsCloak = false;
				if (Data.IdleAnim.RemoveInCloak)
				{
					CreateIdleAnim();
				}
				else if (Data.IdleAnim.TranslucentInCloak)
				{
					// 恢复不透明
					pAnim->AnimFlags = animFlags;
				}
			}
			break;
		default:
			// 进入隐形或处于正在隐\显形的过程中
			if (!ownerIsCloak)
			{
				ownerIsCloak = true;
				if (Data.IdleAnim.RemoveInCloak)
				{
					KillIdleAnim();
				}
				else if (Data.IdleAnim.TranslucentInCloak)
				{
					// 半透明
					pAnim->AnimFlags |= BlitterFlags::TransLucent50;
				}
			}
			break;
		}
	}
}

void Animation::OnRemove()
{
	KillIdleAnim();
}

void Animation::OnReceiveDamage(args_ReceiveDamage* args)
{
	// 受击动画
	if (Data.HitAnim.Enable)
	{
		AnimTypeClass* pAnimType = AnimTypeClass::Find(Data.HitAnim.Type.c_str());
		if (pAnimType)
		{
			OffsetData offsetData = Data.HitAnim.Offset;
			CoordStruct location = GetRelativeLocation(pObject, offsetData).Location;
			AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
			if (pBullet)
			{
				SetAnimOwner(pAnim, pBullet);
				SetAnimCreater(pAnim, pBullet);
			}
			else if (pTechno)
			{
				SetAnimOwner(pAnim, pTechno);
				SetAnimCreater(pAnim, pTechno);
			}
			ShowAnim(pAnim, Data.IdleAnim.Visibility);
			// 设置动画的附着对象，由动画自身去位移
			AnimStatus* status = GetStatus<AnimExt, AnimStatus>(pAnim);
			status->pAttachOwner = pObject;
			status->AttachToObject(pObject, offsetData);
		}
	}
}
