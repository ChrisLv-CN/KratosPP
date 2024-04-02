#include "DamageReactionState.h"

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Scripts.h>

#include <Extension/WarheadTypeExt.h>

#include <Ext/BulletType/BulletStatus.h>

DamageReactionEntity DamageReactionState::GetDataEntity()
{
	DamageReactionEntity data = Data.Data;
	if (_isElite)
	{
		data = Data.EliteData;
	}
	return data;
}

void DamageReactionState::ActionOnce()
{
	_count++;
	DamageReactionEntity data = GetDataEntity();
	ForceDone = data.ActiveOnce;
	_delay = data.Delay;
	if (_delay > 0)
	{
		_delayTimer.Start(_delay);
	}
}

bool DamageReactionState::IsReady()
{
	return !IsDone() && Timeup();
}

bool DamageReactionState::Timeup()
{
	return _delay <= 0 || _delayTimer.Expired();
}

bool DamageReactionState::IsDone()
{
	DamageReactionEntity data = GetDataEntity();
	return data.TriggeredTimes > 0 && _count >= data.TriggeredTimes;
}

bool DamageReactionState::CanPlayAnim()
{
	return _animDelay <= 0 || _animDelayTimer.Expired();
}

void DamageReactionState::OnStart()
{
	_count = 0;
	_delay = 0;
	_delayTimer.Stop();
	_animDelay = 0;
	_animDelayTimer.Stop();
	_isElite = pTechno->Veterancy.IsElite();
}

void DamageReactionState::OnUpdate()
{
	if (IsDone() || ForceDone)
	{
		End();
		return;
	}
	bool isElite = pTechno->Veterancy.IsElite();
	if (isElite != _isElite)
	{
		// 重置计数器
		DamageReactionEntity data = Data.Data;
		if (isElite)
		{
			data = Data.EliteData;
		}
		if (data.ResetTimes)
		{
			_count = 0;
		}
	}
	_isElite = isElite;
}

void DamageReactionState::OnReceiveDamage(args_ReceiveDamage* args)
{
	// 无视防御的真实伤害不做任何响应
	if (!args->IgnoreDefenses)
	{
		WarheadTypeClass* pWH = args->WH;
		WarheadTypeExt::TypeData* whData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
		if (!whData->IgnoreDamageReaction)
		{
			DamageReactionEntity data = GetDataEntity();
			std::vector<DamageReactionMode> ignoreModes = whData->IgnoreDamageReactionModes;
			if (IsReady() && data.Enable && Bingo(data.Chance) && data.WarheadOnMark(pWH->ID))
			{
				int damage = *args->Damage;
				bool action = false;
				switch (data.Mode)
				{
				case DamageReactionMode::REDUCE:
				{
					action = ignoreModes.empty() || std::find(ignoreModes.begin(), ignoreModes.end(), DamageReactionMode::REDUCE) == ignoreModes.end();
					if (action)
					{
						// 调整伤害系数
						*args->Damage = (int)(damage * data.ReducePercent);
					}
					break;
				}
				case DamageReactionMode::FORTITUDE:
				{
					if (damage > data.MaxDamage)
					{
						action = ignoreModes.empty() || std::find(ignoreModes.begin(), ignoreModes.end(), DamageReactionMode::FORTITUDE) == ignoreModes.end();
						if (action)
						{
							// 伤害大于阈值，降低为固定值
							*args->Damage = data.MaxDamage;
						}
					}
					break;
				}
				case DamageReactionMode::PREVENT:
				{
					// 伤害大于血量，致死，消除伤害
					int realDamage = GetRealDamage(pTechno->GetTechnoType()->Armor, damage, pWH, args->IgnoreDefenses, args->DistanceToEpicenter);
					if (realDamage >= pTechno->Health)
					{
						action = ignoreModes.empty() || std::find(ignoreModes.begin(), ignoreModes.end(), DamageReactionMode::PREVENT) == ignoreModes.end();
						if (action)
						{
							// 回避致命伤害
							*args->Damage = 0;
						}
					}
					break;
				}
				default:
				{
					action = ignoreModes.empty() || std::find(ignoreModes.begin(), ignoreModes.end(), DamageReactionMode::EVASION) == ignoreModes.end();
					if (action)
					{
						// 成功闪避，消除伤害
						*args->Damage = 0;
					}
					break;
				}
				}
				if (action)
				{
					// 成功激活一次响应
					ActionOnce();
					// 附加AE
					if (!data.TriggeredAttachEffects.empty())
					{
						if (AttachEffect* aem = _gameObject->GetComponent<AttachEffect>())
						{
							if (data.TriggeredAttachEffectsFromAttacker)
							{
								aem->Attach(data.TriggeredAttachEffects, data.TriggeredAttachEffectChances, false, args->Attacker, args->SourceHouse);
							}
							else
							{
								aem->Attach(data.TriggeredAttachEffects, data.TriggeredAttachEffectChances, false);
							}
						}
					}
					// 播放响应动画
					if (IsNotNone(data.Anim) && CanPlayAnim())
					{
						if (AnimTypeClass* pAnimType = AnimTypeClass::Find(data.Anim.c_str()))
						{
							CoordStruct location = pTechno->GetCoords();
							if (!data.AnimFLH.IsEmpty())
							{
								location = GetFLHAbsoluteCoords(pTechno, data.AnimFLH, false);
							}
							AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
							SetAnimOwner(pAnim, pTechno);
						}
						_animDelay = data.AnimDelay;
						if (_animDelay > 0)
						{
							_animDelayTimer.Start(_animDelay);
						}
					}
					// 显示DamageReaction
					if (data.ActionText)
					{
						DamageText* damageText = _gameObject->GetComponent<DamageText>();
						DamageTextData* textType = nullptr;
						if (damageText && !damageText->SkipDrawDamageText(pWH, textType))
						{
							DamageTextEntity textData;
							switch (data.TextStyle)
							{
							case DamageTextStyle::DAMAGE:
								textData = textType->Damage;
								break;
							case DamageTextStyle::REPAIR:
								textData = textType->Repair;
								break;
							default:
								if (*args->Damage >= 0)
								{
									textData = textType->Damage;
								}
								else
								{
									textData = textType->Repair;
								}
								break;
							}
							if (!textData.Hidden)
							{
								CoordStruct location = pTechno->GetCoords();
								DamageTextEntity* temp = &textData;
								if (IsNotNone(data.CustomSHP))
								{
									// 自定义SHP
									temp->UseSHP = true;
									temp->SHPDrawStyle = SHPDrawStyle::TEXT;
									temp->SHPFileName = data.CustomSHP;
									temp->ZeroFrameIndex = data.CustomSHPIndex;
									damageText->OrderDamageText(L"WWSB", location, temp);
								}
								else if (IsNotNone(data.CustomText))
								{
									// 自定义文字
									temp->UseSHP = false;
									std::wstring text = String2WString(data.CustomText);
									damageText->OrderDamageText(text, location, temp);
								}
								else
								{
									// 使用默认设置
									temp->UseSHP = false;
									std::wstring longText;
									for (auto it : LongTextStrings)
									{
										if (it.second == data.DefaultText)
										{
											longText = it.first;
											break;
										}
									}
									damageText->OrderDamageText(longText, location, temp);
								}
							}
						}
					}
				}
			}
		}
	}
}
