#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

class CombatDamageData : public INIConfig
{
public:
	// Ares
	bool AutoRepel = false;
	bool PlayerAutoRepel = false;

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

	virtual void Read(INIBufferReader* reader) override
	{
		AutoRepel = reader->Get("AutoRepel", AutoRepel);
		PlayerAutoRepel = reader->Get("PlayerAutoRepel", PlayerAutoRepel);

		AllowAnimDamageTakeOverByKratos = reader->Get("AllowAnimDamageTakeOverByKratos", AllowAnimDamageTakeOverByKratos);
		AllowDamageIfDebrisHitWater = reader->Get("AllowDamageIfDebrisHitWater", AllowDamageIfDebrisHitWater);

		AllowAutoPickStandAsTarget = reader->Get("AllowAutoPickStandAsTarget", AllowAutoPickStandAsTarget);
		AllowUnitAsBaseNormal = reader->Get("AllowUnitAsBaseNormal", AllowUnitAsBaseNormal);
		AllowJumpjetAsBaseNormal = reader->Get("AllowJumpjetAsBaseNormal", AllowJumpjetAsBaseNormal);
		AllowStandAsBaseNormal = reader->Get("AllowStandAsBaseNormal", AllowStandAsBaseNormal);

		AllowAIAttackFriendlies = reader->Get("AllowAIAttackFriendlies", AllowAIAttackFriendlies);
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

	virtual void Read(INIBufferReader* reader) override
	{
		DeactivateDimEMP = reader->Get("DeactivateDimEMP", DeactivateDimEMP);
		DeactivateDimPowered = reader->Get("DeactivateDimPowered", DeactivateDimPowered);

		AllowMakeVoxelDebrisByKratos = reader->Get("AllowMakeVoxelDebrisByKratos", AllowMakeVoxelDebrisByKratos);
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
