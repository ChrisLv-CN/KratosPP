#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

class CombatDamageData : public INIConfig
{
public:
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

	virtual void Read(INIBufferReader* ini) override
	{
		AllowAnimDamageTakeOverByKratos = ini->Get("AllowAnimDamageTakeOverByKratos", AllowAnimDamageTakeOverByKratos);
		AllowDamageIfDebrisHitWater = ini->Get("AllowDamageIfDebrisHitWater", AllowDamageIfDebrisHitWater);

		AllowAutoPickStandAsTarget = ini->Get("AllowAutoPickStandAsTarget", AllowAutoPickStandAsTarget);
		AllowUnitAsBaseNormal = ini->Get("AllowUnitAsBaseNormal", AllowUnitAsBaseNormal);
		AllowJumpjetAsBaseNormal = ini->Get("AllowJumpjetAsBaseNormal", AllowJumpjetAsBaseNormal);
		AllowStandAsBaseNormal = ini->Get("AllowStandAsBaseNormal", AllowStandAsBaseNormal);

		AllowAIAttackFriendlies = ini->Get("AllowAIAttackFriendlies", AllowAIAttackFriendlies);
	}
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
	// Ares
	float DeactivateDimEMP = 0.8f;
    float DeactivateDimPowered = 0.5f;

	// Kratos
	bool AllowMakeVoxelDebrisByKratos = true;

	virtual void Read(INIBufferReader* ini) override
	{
		DeactivateDimEMP = ini->Get("DeactivateDimEMP", DeactivateDimEMP);
		DeactivateDimPowered = ini->Get("DeactivateDimPowered", DeactivateDimPowered);

		AllowMakeVoxelDebrisByKratos = ini->Get("AllowMakeVoxelDebrisByKratos", AllowMakeVoxelDebrisByKratos);
	}

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
