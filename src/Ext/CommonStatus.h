#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

class CombatDamageData : public INIConfig
{
public:
	virtual void Read(INIBufferReader* ini) override
	{
		this->AllowAnimDamageTakeOverByKratos = ini->Get("AllowAnimDamageTakeOverByKratos", AllowAnimDamageTakeOverByKratos);
		this->AllowDamageIfDebrisHitWater = ini->Get("AllowDamageIfDebrisHitWater", AllowDamageIfDebrisHitWater);

		this->AllowAutoPickStandAsTarget = ini->Get("AllowAutoPickStandAsTarget", AllowAutoPickStandAsTarget);
		this->AllowUnitAsBaseNormal = ini->Get("AllowUnitAsBaseNormal", AllowUnitAsBaseNormal);
		this->AllowJumpjetAsBaseNormal = ini->Get("AllowJumpjetAsBaseNormal", AllowJumpjetAsBaseNormal);
		this->AllowStandAsBaseNormal = ini->Get("AllowStandAsBaseNormal", AllowStandAsBaseNormal);

		this->AllowAIAttackFriendlies = ini->Get("AllowAIAttackFriendlies", AllowAIAttackFriendlies);
	}
	// 动画
	bool AllowAnimDamageTakeOverByKratos = true;
	bool AllowDamageIfDebrisHitWater = true;

	// 替身
	bool AllowAutoPickStandAsTarget = true;
	bool AllowUnitAsBaseNormal = false;
	bool AllowJumpjetAsBaseNormal = false;
	bool AllowStandAsBaseNormal = false;

	// AI
	bool AllowAIAttackFriendlies = false;
};

class CombatDamage
{
public:
	static CombatDamageData* Data()
	{
		if (!_data)
		{
			_data = INI::GetConfig<CombatDamageData>(INI::Rules, INI::SectionCombatDamage)->Data;
		}
		return _data;
	};

private:
	static CombatDamageData* _data;
};

class AudioVisualData : public INIConfig
{
public:
	virtual void Read(INIBufferReader* ini) override
	{
		this->AllowMakeVoxelDebrisByKratos = ini->Get("AllowMakeVoxelDebrisByKratos", AllowMakeVoxelDebrisByKratos);
	}

	bool AllowMakeVoxelDebrisByKratos = true;
};

class AudioVisual
{
public:
	static AudioVisualData* Data()
	{
		if (!_data)
		{
			_data = INI::GetConfig<AudioVisualData>(INI::Rules, INI::SectionAudioVisual)->Data;
		}
		return _data;
	};

private:
	static AudioVisualData* _data;
};
