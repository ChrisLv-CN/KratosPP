#include "AnimStatus.h"

void AnimStatus::SetOffset(OffsetData data)
{
	_offsetData = data;
	Offset = _offsetData.Offset;
}

void AnimStatus::OnUpdate()
{
	// 如果有附着对象，就移动动画的位置
	if (!IsDead(pAttachOwner))
	{
		CoordStruct location{};
		if (pAnim->IsBuildingAnim)
		{
			location = pAttachOwner->GetRenderCoords() + Offset;
		}
		else
		{
			if (!_offsetData.Offset.IsEmpty())
			{
				location = GetRelativeLocation(pAttachOwner, _offsetData, Offset).Location;
			}
			else
			{
				location = pAttachOwner->GetCoords() + Offset;
			}
		}
		if (!location.IsEmpty())
		{
			pAnim->SetLocation(location);
		}
	}
	OnUpdate_Visibility();
	OnUpdate_Damage();
	OnUpdate_SpawnAnims();
}

void AnimStatus::OnUpdate_Visibility() {}
void AnimStatus::OnUpdate_Damage() {}
void AnimStatus::OnUpdate_SpawnAnims() {}

void AnimStatus::OnLoop()
{
	OnLoop_SpawnAnims();
}

void AnimStatus::OnDone()
{
	OnDone_SpawnAnims();
}

void AnimStatus::OnNext(AnimTypeClass* pNext)
{
	// 动画next会换类型，刷新设置

	OnNext_SpawnAnims(pNext);
}

void AnimStatus::OnLoop_SpawnAnims() {}
void AnimStatus::OnDone_SpawnAnims() {}
void AnimStatus::OnNext_SpawnAnims(AnimTypeClass* pNext) {}

