#include "..\TechnoStatus.h"

#include <Common/INI/INI.h>
#include <Ext/Helper.h>
#include <Ext/PrintTextManager.h>

bool TechnoStatus::SkipDrawDamageText(WarheadTypeClass* pWH, DamageTextData*& damageTextType)
{
	if (!_skipDamageText && pWH && !pTechno->InLimbo && !IsCloaked(pTechno))
	{
		damageTextType = INI::GetConfig<DamageTextData>(INI::Rules, pWH->ID)->Data;
		return damageTextType->Hidden;
	}
	return true;
}

void TechnoStatus::OrderDamageText(std::wstring text, CoordStruct location, DamageText*& data)
{
	int x = _random.RandomRanged(data->XOffset.X, data->XOffset.Y);
	int y = _random.RandomRanged(data->YOffset.X, data->YOffset.Y) - 15; // 离地高度
	Point2D offset{ x, y };
	// 横向锚点修正
	int length = text.size() / 2;
	if (data->UseSHP)
	{
		offset.X -= data->ImageSize.X * length;
	}
	else
	{
		offset.X -= PrintTextManager::GetFontSize().X * length;
	}
	PrintTextManager::AddRollingText(text, location, offset, data->RollSpeed, data->Duration, *data);
}

void TechnoStatus::OnUpdate_DamageText()
{
	CoordStruct location = pTechno->GetCoords();
	int frame = Unsorted::CurrentFrame;
	// 伤害数字
	for (auto it = _damageCache.begin(); it != _damageCache.end();)
	{
		if (frame - it->second.StartFrame >= it->first->Rate)
		{
			std::wstring text = L"-" + std::to_wstring(it->second.Value);
			DamageText* data = it->first;
			OrderDamageText(text, location, data);
			it = _damageCache.erase(it);
		}
		else
		{
			it++;
		}
	}
	// 治疗数字
	for (auto it = _repairCache.begin(); it != _repairCache.end();)
	{
		if (frame - it->second.StartFrame >= it->first->Rate)
		{
			std::wstring text = L"+" + std::to_wstring(it->second.Value);
			DamageText* data = it->first;
			OrderDamageText(text, location, data);
			it = _repairCache.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void TechnoStatus::OnReceiveDamageEnd_DamageText(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	DamageTextData* whDamageTextType = nullptr;
	if (SkipDrawDamageText(pWH, whDamageTextType))
	{
		return;
	}
	else
	{
		_skipDamageText = false;
	}
	std::wstring text{};
	DamageText* data = nullptr;
	int damage = *pRealDamage;
	int damageValue = 0;
	int repairValue = 0;
	if (damage > 0)
	{
		data = &whDamageTextType->Damage;
		if (!data->Hidden)
		{
			damageValue += damage;
			text = L"-" + std::to_wstring(damage);
		}
	}
	else if (damage < 0)
	{
		data = &whDamageTextType->Repair;
		if (!data->Hidden)
		{
			repairValue += damage;
			text = L"-" + std::to_wstring(-damage);
		}
	}
	if (text.empty() || !data || data->Hidden)
	{
		return;
	}
	if (data->Detail || damageState == DamageState::NowDead)
	{
		// 直接下单
		CoordStruct location = pTechno->GetCoords();
		OrderDamageText(text, location, data);
	}
	else
	{
		// 写入缓存
		if (damageValue > 0)
		{
			auto it = _damageCache.find(data);
			if (it != _damageCache.end())
			{
				_damageCache[data].Add(damageValue);
			}
			else
			{
				DamageTextCache cache{ damageValue };
				_damageCache[data] = cache;
			}
		}
		else if (repairValue > 0)
		{
			auto it = _repairCache.find(data);
			if (it != _repairCache.end())
			{
				_repairCache[data].Add(repairValue);
			}
			else
			{
				DamageTextCache cache{ repairValue };
				_repairCache[data] = cache;
			}
		}
	}
};

