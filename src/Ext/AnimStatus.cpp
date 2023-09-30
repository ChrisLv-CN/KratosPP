#include "AnimStatus.h"

#include <Ext/Helper/CastEx.h>
#include <Ext/ExpandAnimsManager.h>
#include <Ext/FireSuperManager.h>

bool AnimStatus::TryGetCreater(TechnoClass*& pTechno)
{
	pTechno = pCreater;
	return pTechno != nullptr;
}

void AnimStatus::SetOffset(OffsetData data)
{
	_offsetData = data;
	Offset = _offsetData.Offset;
}

AnimDamageData* AnimStatus::GetAnimDamageData()
{
	if (!_animDamageData)
	{
		_animDamageData = INI::GetConfig<AnimDamageData>(INI::Art, pAnim->Type->ID)->Data;
	}
	return _animDamageData;
}

void AnimStatus::Explosion_Damage(bool isBounce, bool bright)
{
	if (pAnim)
	{
		AnimTypeClass* pAnimType = pAnim->Type;
		if (pAnimType)
		{
			CoordStruct location = pAnim->GetCoords();
			if (isBounce)
			{
				location = pAnim->Bounce.GetCoords();
			}
			int damage = (int)pAnimType->Damage;
			if (damage != 0 || (!IsNotNone(GetAnimDamageData()->Weapon) && GetAnimDamageData()->UseWeaponDamage))
			{
				// 制造伤害
				std::string weaponType = GetAnimDamageData()->Weapon;
				WarheadTypeClass* pWH = pAnimType->Warhead;
				// 检查动画类型有没有写弹头
				if (IsNotNone(weaponType))
				{
					// 用武器
					WeaponTypeClass* pWeapon = WeaponTypeClass::Find(weaponType.c_str());
					if (pWeapon)
					{
						// 使用武器的伤害数值
						if (GetAnimDamageData()->UseWeaponDamage)
						{
							damage = pWeapon->Damage;
						}
						if (damage != 0)
						{
							if (_damageDelayTimer.Expired())
							{
								// Logger.Log($"{Game.CurrentFrame} - 动画 {pAnim} [{pAnimType->ID}] 用武器播放伤害 TypeDamage = {damage}, AnimDamage = {pAnim->Damage}, Weapon = {weaponType}");
								pWH = pWeapon->Warhead;
								bool isBright = bright || pWeapon->Bright; // 原游戏中弹头上的bright是无效的
								BulletTypeClass* pBulletType = pWeapon->Projectile;
								BulletClass* pBullet = pBulletType->CreateBullet(nullptr, pCreater, damage, pWH, pWeapon->Speed, isBright);
								pBullet->WeaponType = pWeapon;
								SetSourceHouse(pBullet, pAnim->Owner);
								pBullet->Detonate(location);
								pBullet->UnInit();
								_damageDelayTimer.Start(GetAnimDamageData()->Delay);
							}
						}
					}
				}
				else if (pWH)
				{
					// 用弹头
					if (_damageDelayTimer.Expired())
					{
						// Logger.Log($"{Game.CurrentFrame} - 动画 {pAnim} [{pAnimType->ID}] 用弹头播放伤害 TypeDamage = {damage}, AnimDamage = {pAnim->Damage}, Warhead = {pAnimType->Warhead}");
						MapClass::DamageArea(location, damage, pCreater, pWH, true, pAnim->Owner);
						_damageDelayTimer.Start(GetAnimDamageData()->Delay);
						if (bright)
						{
							MapClass::FlashbangWarheadAt(damage, pWH, location);
						}
						// 播放弹头动画
						if (GetAnimDamageData()->PlayWarheadAnim)
						{
							LandType landType = LandType::Clear;
							if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
							{
								landType = pCell->LandType;
							}
							AnimTypeClass* pWHAnimType = MapClass::SelectDamageAnimation(damage, pWH, landType, location);
							// Logger.Log($"{Game.CurrentFrame} - Anim {pAnim} [{pAnimType->ID}] play warhead's Anim [{(pWHAnimType.IsNull ? "null" : pWHAnimType->ID)}]");
							if (pWHAnimType)
							{
								AnimClass* pWHAnim = GameCreate<AnimClass>(pWHAnimType, location);
								pWHAnim->Owner = pAnim->Owner;
							}
						}
					}
				}

			}
		}
	}
}

ExpireAnimData* AnimStatus::GetExpireAnimData()
{
	if (!_expireAnimData)
	{
		_expireAnimData = INI::GetConfig<ExpireAnimData>(INI::Art, pAnim->Type->ID)->Data;
	}
	return _expireAnimData;
}


bool AnimStatus::OverrideExpireAnimOnWater()
{

	if (IsNotNone(GetExpireAnimData()->ExpireAnimOnWater))
	{
		// Logger.Log($"{Game.CurrentFrame} 试图接管 落水动画 {animType}");
		AnimTypeClass* pNewType = AnimTypeClass::Find(GetExpireAnimData()->ExpireAnimOnWater.c_str());
		if (pNewType)
		{
			// Logger.Log($"{Game.CurrentFrame} 试图创建新的落水动画 {animType}");
			AnimClass* pNewAnim = GameCreate<AnimClass>(pNewType, pAnim->GetCoords());
			pNewAnim->Owner = pAnim->Owner;
		}
		return true; // skip create anim
	}
	return false;
}

PaintballData* AnimStatus::GetPaintballData()
{
	if (!_paintballData)
	{
		_paintballData = INI::GetConfig<PaintballData>(INI::Art, pAnim->Type->ID)->Data;
	}
	return _paintballData;
}

void AnimStatus::DrawSHP_Paintball(REGISTERS* R)
{
	if (GetPaintballData()->Enable)
	{
		if (GetPaintballData()->ChangeColor)
		{
			R->EBP(GetPaintballData()->Color2);
		}
		if (GetPaintballData()->ChangeBright)
		{
			GET_STACK(unsigned int, bright, 56);
			R->Stack(56, GetBright(bright, GetPaintballData()->BrightMultiplier));
		}
	}
}

PlaySuperData* AnimStatus::GetPlaySuperData()
{
	if (!_playSuperData)
	{
		_playSuperData = INI::GetConfig<PlaySuperData>(INI::Art, pAnim->Type->ID)->Data;
	}
	return _playSuperData;
}

bool AnimStatus::GetInvisible(Relation visibility)
{
	if (pAnim->Owner)
	{
		Relation relation = GetRelationWithPlayer(pAnim->Owner);
		return !(visibility & relation);
	}
	return false;
}

void AnimStatus::UpdateVisibility(Relation visibility)
{
	pAnim->Invisible = GetInvisible(visibility);
	_initInvisibleFlag = true;
}

SpawnAnimsData* AnimStatus::GetSpawnAnimsData()
{
	if (!_spawnAnimsData)
	{
		_spawnAnimsData = INI::GetConfig<SpawnAnimsData>(INI::Art, pAnim->Type->ID)->Data;
	}
	return _spawnAnimsData;
}

void AnimStatus::ResetLoopSpawn()
{
	_initSpawnFlag = false;
	_spawnInitDelayTimer.Stop();
	_spawnDelayTimer.Stop();
	_spawnCount = 0;
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
	OnUpdate_PlaySuper();
}

void AnimStatus::OnUpdate_Visibility()
{
	// 断言第一次执行update时，所属已被设置
	if (!_initInvisibleFlag)
	{
		// 初始化
		AnimVisibilityData* data = INI::GetConfig<AnimVisibilityData>(INI::Art, pAnim->Type->ID)->Data;
		UpdateVisibility(data->Visibility);
	}
}

void AnimStatus::OnUpdate_Damage()
{
	if (pAnim)
	{
		if (!_initDamageDelayFlag)
		{
			_initDamageDelayFlag = true;
			_damageDelayTimer.Start(GetAnimDamageData()->Delay);
		}
		if (!_createrIsDeadth)
		{
			if (!pCreater)
			{
				if (!(_createrIsDeadth = !GetAnimDamageData()->KillByCreater))
				{
					TechnoClass* pCreater = nullptr;
					ObjectClass* pOwner = pAttachOwner;
					if (!pOwner)
					{
						pOwner = pAnim->OwnerObject;
					}
					if (pOwner)
					{
						TechnoClass* pTechno = nullptr;
						BulletClass* pBullet = nullptr;
						if (CastToTechno(pOwner, pTechno))
						{
							pCreater = pTechno;
						}
						else if (CastToBullet(pOwner, pBullet))
						{
							if (BulletStatus* bulletStatus = GetStatus<BulletExt, BulletStatus>(pBullet))
							{
								pCreater = bulletStatus->pSource;
							}
						}
					}
					_createrIsDeadth = IsDead(pCreater);
				}
			}
			else if ((_createrIsDeadth = IsDead(pCreater)) == true)
			{
				pCreater = nullptr;
			}
		}
	}
}

void AnimStatus::OnUpdate_SpawnAnims()
{
	if (GetSpawnAnimsData()->Enable && GetSpawnAnimsData()->TriggerOnStart && GetSpawnAnimsData()->Count != 0)
	{
		if (!_initSpawnFlag)
		{
			ResetLoopSpawn();
			_initSpawnFlag = true;
			int _initDelay = 0;
			if ((_initDelay = GetSpawnAnimsData()->GetInitDelay()) > 0)
			{
				_spawnInitDelayTimer.Start(_initDelay);
			}
			else
			{
				_spawnInitDelayTimer.Stop();
			}
		}
		// 释放动画
		if ((GetSpawnAnimsData()->Count < 0 || _spawnCount < GetSpawnAnimsData()->Count) && _spawnInitDelayTimer.Expired() && _spawnDelayTimer.Expired())
		{
			ExpandAnimsManager::PlayExpandAnims(*GetSpawnAnimsData(), pAnim->GetCoords(), pAnim->Owner);
			_spawnCount++;
			int delay = 0;
			if ((delay = GetSpawnAnimsData()->GetDelay()) > 0)
			{
				_spawnDelayTimer.Start(delay);
			}
		}
	}
	else
	{
		ResetLoopSpawn();
	}
}

void AnimStatus::OnUpdate_PlaySuper()
{
	if (!_playSuperFlag)
	{
		_playSuperFlag = true;
		if (GetPlaySuperData()->Enable && GetPlaySuperData()->LaunchMode == PlaySuperWeaponMode::CUSTOM)
		{
			CoordStruct location = pAnim->GetCoords();
			FireSuperManager::Order(pAnim->Owner, location, GetPlaySuperData()->Data);
		}
	}
}

void AnimStatus::OnLoop()
{
	OnLoop_SpawnAnims();
	OnLoop_PlaySuper();
}

void AnimStatus::OnLoop_SpawnAnims()
{
	if (GetSpawnAnimsData()->Enable && GetSpawnAnimsData()->TriggerOnLoop)
	{
		ExpandAnimsManager::PlayExpandAnims(*GetSpawnAnimsData(), pAnim->GetCoords(), pAnim->Owner);
	}
}

void AnimStatus::OnLoop_PlaySuper()
{
	if (GetPlaySuperData()->Enable && GetPlaySuperData()->LaunchMode == PlaySuperWeaponMode::LOOP)
	{
		CoordStruct location = pAnim->GetCoords();
		FireSuperManager::Launch(pAnim->Owner, location, GetPlaySuperData()->Data);
	}
}

void AnimStatus::OnDone()
{
	OnDone_SpawnAnims();
	OnDone_PlaySuper();
}

void AnimStatus::OnDone_SpawnAnims()
{
	if (GetSpawnAnimsData()->Enable && GetSpawnAnimsData()->TriggerOnDone)
	{
		ExpandAnimsManager::PlayExpandAnims(*GetSpawnAnimsData(), pAnim->GetCoords(), pAnim->Owner);
	}
}

void AnimStatus::OnDone_PlaySuper()
{
	_playSuperFlag = false;
	if (GetPlaySuperData()->Enable)
	{
		switch (GetPlaySuperData()->LaunchMode)
		{
		case PlaySuperWeaponMode::LOOP:
		case PlaySuperWeaponMode::DONE:
			CoordStruct targetPos = pAnim->GetCoords();
			FireSuperManager::Launch(pAnim->Owner, targetPos, GetPlaySuperData()->Data);
			break;
		}
	}
}

void AnimStatus::OnNext(AnimTypeClass* pNext)
{
	// 动画next会换类型，刷新设置
	_expireAnimData = nullptr;
	_paintballData = nullptr;
	_playSuperData = nullptr;
	_playSuperFlag = false;

	OnNext_SpawnAnims(pNext);
}

void AnimStatus::OnNext_SpawnAnims(AnimTypeClass* pNext)
{
	ResetLoopSpawn();
	if (GetSpawnAnimsData()->Enable && GetSpawnAnimsData()->TriggerOnNext)
	{
		ExpandAnimsManager::PlayExpandAnims(*GetSpawnAnimsData(), pAnim->GetCoords(), pAnim->Owner);
	}
	_spawnAnimsData = nullptr;
}

