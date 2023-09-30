#pragma once

#include <string>

#include <Common/INI/INIConfig.h>

class AnimDamageData : public INIConfig
{
public:

	int Damage = 0; // 动画伤害
	int InitDelay = 0; // 动画伤害初始延迟
	int Delay = 0; // 动画伤害延迟

	bool KillByCreater = false; // 动画制造伤害传递攻击者为动画的创建者

	std::string Warhead{ "" }; // 使用弹头制造伤害
	bool PlayWarheadAnim = false; // 播放弹头动画

	std::string Weapon{ "" }; // 使用武器制造伤害
	bool UseWeaponDamage = false; // 使用武器的伤害而不是动画的伤害

	virtual void Read(INIBufferReader* reader) override
	{
		Damage = reader->Get("Damage", Damage);
		Delay = reader->Get("Damage.Delay", Delay);
		// Ares 习惯，InitDelay 与 Delay相同
		InitDelay = reader->Get("Damage.InitDelay", Delay);

		KillByCreater = reader->Get("Damage.KillByCreater", KillByCreater);

		Warhead = reader->Get("Warhead", Warhead);
		PlayWarheadAnim = reader->Get("Warhead.PlayAnim", PlayWarheadAnim);

		Weapon = reader->Get("Weapon", Weapon);
		UseWeaponDamage = reader->Get("Weapon.AllowDamage", UseWeaponDamage);
	}

};

