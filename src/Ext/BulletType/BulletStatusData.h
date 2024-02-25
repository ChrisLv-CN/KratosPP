#pragma once

#include <Common/INI/INIConfig.h>

/// @brief 抛射体的生存属性
struct BulletLife
{
public:
	bool Interceptable = false; // 可被伤害
	int Strength = -1;			// 自定义血量

	int Health = 1;			 // 生命值
	bool IsDetonate = false; // 已损毁
	bool IsHarmless = false; // 无害
	bool SkipAE = false;	 // 爆炸不赋予AE

	void Read(INIBufferReader* ini)
	{
		this->Interceptable = ini->Get("Interceptable", Interceptable);
		this->Strength = ini->Get("Strength", Strength);
		if (Strength > 0)
		{
			this->Health = Strength;
		}
	}

	/// @brief 直接摧毁
	/// @param harmless 无害
	/// @param skipAE 不赋予AE
	void Detonate(bool harmless = false, bool skipAE = false)
	{
		this->Health = -1;
		this->IsDetonate = true;
		this->IsHarmless = harmless;
		this->SkipAE = skipAE;
	}

	/// @brief 收到伤害
	/// @param damage 伤害数值
	/// @param harmless 无害
	/// @param skipAE 不赋予AE
	void TakeDamage(int damage, bool harmless, bool skipAE = false)
	{
		this->Health -= damage;
		this->IsDetonate = this->Health <= 0;
		this->IsHarmless = harmless;
		if (IsDetonate)
		{
			this->SkipAE = skipAE;
		}
	}
};

/// @brief 对抛射体的伤害属性
struct BulletDamage
{
public:
	int Damage = 0;		   // 伤害
	bool Eliminate = true; // 一击必杀
	bool Harmless = false; // 和平处置
};


/// @brief 记录抛射体的实时状态
struct RecordBulletStatus
{
public:
	int Health;
	int Speed;
	BulletVelocity Velocity;
	bool CourseLocked;
};
