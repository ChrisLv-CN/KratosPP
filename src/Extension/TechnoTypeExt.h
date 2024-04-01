#pragma once

#include <TechnoTypeClass.h>

#include "TypeExtension.h"

class TechnoTypeExt : public TypeExtension<TechnoTypeClass, TechnoTypeExt>
{
public:
	/// @brief 储存一些通用设置或者其他平台的设置
	class TypeData : public INIConfig
	{
	public:
		// Ares
		bool AllowCloakable = true;
		bool Carryall = false;
		int CarryallSizeLimit = -1;
		bool CarryallAllowed = true;

		// Phobos
		CoordStruct TurretOffset = CoordStruct::Empty;
		std::string WarpIn{ "" };
		std::string WarpOut{ "" };
		std::string WarpAway{ "" };
		int ChronoDelay = 60;
		int ChronoDistanceFactor = 32;
		bool ChronoTrigger = true;
		int ChronoMinimumDelay = 0;
		int ChronoRangeMinimum = 0;

		// Kratos
		CoordStruct CarryallOffset{ 0, 0, Unsorted::LevelHeight };
		std::string CarryallImage{ "" };

		virtual void Read(INIBufferReader* reader) override
		{
			AllowCloakable = reader->Get("Cloakable.Allowed", AllowCloakable);
			Carryall = reader->Get("Carryall", Carryall);
			CarryallSizeLimit = reader->Get("Carryall.SizeLimit", CarryallSizeLimit);
			CarryallAllowed = reader->Get("Carryall.Allowed", CarryallAllowed);

			TurretOffset = reader->Get("TurretOffset", TurretOffset);

			WarpIn = reader->Get("WarpIn", WarpIn);
			WarpOut = reader->Get("WarpOut", WarpOut);
			WarpAway = reader->Get("WarpAway", WarpAway);
			// 在游戏开始后再访问，读取全局没问题
			RulesClass* rules = RulesClass::Instance;
			ChronoDelay = reader->Get("ChronoDelay", rules->ChronoDelay);
			ChronoDistanceFactor = reader->Get("ChronoDistanceFactor", rules->ChronoDistanceFactor);
			ChronoTrigger = reader->Get("ChronoTrigger", rules->ChronoTrigger);
			ChronoMinimumDelay = reader->Get("ChronoMinimumDelay", rules->ChronoMinimumDelay);
			ChronoRangeMinimum = reader->Get("ChronoRangeMinimum", rules->ChronoRangeMinimum);

			CarryallOffset = reader->Get("Carryall.Offset", CarryallOffset);
			CarryallImage = reader->Get("Carryall.Image", CarryallImage);
		}
	};

	static constexpr DWORD Canary = 0x11111111;
	// static constexpr size_t ExtPointerOffset = 0xDF4;

	static TechnoTypeExt::ExtContainer ExtMap;
};
