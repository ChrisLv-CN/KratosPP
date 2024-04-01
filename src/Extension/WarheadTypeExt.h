#pragma once
#include <string>
#include <vector>
#include <stack>

#include <Conversions.h>
#include <GeneralDefinitions.h>
#include <GeneralStructures.h>
#include <WarheadTypeClass.h>
#include <CCINIClass.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/StringEx.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/StateType/State/DamageReactionData.h>

#include "TypeExtension.h"

enum class ExpLevel
{
	None = 0, Rookie = 1, Veteran = 2, Elite = 3
};

template <>
inline bool Parser<ExpLevel>::TryParse(const char* pValue, ExpLevel* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'R':
		if (outValue)
		{
			*outValue = ExpLevel::Rookie;
		}
		return true;
	case 'V':
		if (outValue)
		{
			*outValue = ExpLevel::Veteran;
		}
		return true;
	case 'E':
		if (outValue)
		{
			*outValue = ExpLevel::Elite;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = ExpLevel::None;
		}
		return true;
	}
}

/*
enum class Armor : unsigned int
{
	None = 0,
	Flak = 1,
	Plate = 2,
	Light = 3,
	Medium = 4,
	Heavy = 5,
	Wood = 6,
	Steel = 7,
	Concrete = 8,
	Special_1 = 9,
	Special_2 = 10
};*/

static std::map<std::string, Armor> ArmorTypeStrings
{
	{ "none", Armor::None },
	{ "flak", Armor::Flak },
	{ "plate", Armor::Plate },
	{ "light", Armor::Light },
	{ "medium", Armor::Medium },
	{ "heavy", Armor::Heavy },
	{ "wood", Armor::Wood },
	{ "steel", Armor::Steel },
	{ "concrete", Armor::Concrete },
	{ "special_1", Armor::Special_1 },
	{ "special_2", Armor::Special_2 }
};

struct AresVersus : public WarheadFlags
{
public:
	AresVersus(double versus = 1.0, bool forceFire = true, bool retaliate = true, bool passiveAcquire = true) : Versus(versus), WarheadFlags(forceFire, retaliate, passiveAcquire) {}

	bool operator ==(const AresVersus& RHS) const {
		return (CLOSE_ENOUGH(this->Versus, RHS.Versus));
	}

	void Parse(const char* str) {
		this->Versus = Conversions::Str2Armor(str, this);
	}

	void Read(INIBufferReader* reader, std::string title, std::string armor)
	{
		Versus = reader->Get(title + armor, Versus);
		ForceFire = reader->Get(title + armor + ".ForceFire", ForceFire);
		Retaliate = reader->Get(title + armor + ".Retaliate", Retaliate);
		PassiveAcquire = reader->Get(title + armor + ".PassiveAcquire", PassiveAcquire);
	}

	double Versus = 1.0;
};

class WarheadTypeExt : public TypeExtension<WarheadTypeClass, WarheadTypeExt>
{
public:
	/// @brief 储存一些通用设置或者其他平台的设置
	class TypeData : public INIConfig
	{
	public:
		// YR
		std::vector<double> Versus{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }; // YRPP获取的比例全是1，只能自己维护
		// Ares
		std::vector<AresVersus> AresVersusArray{}; // Ares自定义护甲的参数

		bool AffectsOwner = true;
		bool AffectsAllies = true;
		bool AffectsEnemies = true;
		bool EffectsRequireDamage = false;
		bool EffectsRequireVerses = true;
		bool AllowZeroDamage = false;
		std::string PreImpactAnim{ "" };

		// Kratos
		bool AffectInAir = true;
		bool AffectShooter = true;
		bool AffectStand = false;

		bool ClearTarget = false;
		bool ClearDisguise = false;

		Mission ForceMission = Mission::None;

		int ExpCost = 0;
		ExpLevel ExpLevel = ExpLevel::None;

		int ShowMeTheMoney = 0;
		bool ShowMeTheMoneyDisplay = false;

		bool IsToy = false; // 玩具弹头

		bool Lueluelue = false; // 欠揍弹头

		bool Teleporter = false; // 弹头传送标记
		bool Capturer = false; // 弹头黑洞捕获标记
		bool IgnoreRevenge = false; // 不触发复仇
		bool IgnoreDamageReaction = false; // 不触发伤害相应
		std::vector<DamageReactionMode> IgnoreDamageReactionModes{}; // 不触发特定的伤害相应类型
		bool IgnoreStandShareDamage = false; // 替身不分担伤害

		virtual void Read(INIBufferReader* reader) override
		{
			Versus = reader->GetList("Verses", Versus);
			// Ares
			ReadAresVersus(reader);
			bool affectsAllies = true;
			if (reader->TryGet("AffectsAllies", affectsAllies))
			{
				AffectsAllies = affectsAllies;
				AffectsOwner = affectsAllies;
			}
			bool affectsOwner = AffectsOwner;
			if (reader->TryGet("AffectsOwner", affectsOwner))
			{
				AffectsOwner = affectsOwner;
			}
			AffectsEnemies = reader->Get("AffectsEnemies", AffectsEnemies);
			EffectsRequireDamage = reader->Get("EffectsRequireDamage", EffectsRequireDamage);
			EffectsRequireVerses = reader->Get("EffectsRequireVerses", EffectsRequireVerses);
			AllowZeroDamage = reader->Get("AllowZeroDamage", AllowZeroDamage);
			PreImpactAnim = reader->Get("PreImpactAnim", PreImpactAnim);

			// Kratos
			AffectInAir = reader->Get("AffectInAir", AffectInAir);
			AffectShooter = reader->Get("AffectShooter", AffectShooter);
			AffectStand = reader->Get("AffectStand", AffectStand);

			ClearTarget = reader->Get("ClearTarget", ClearTarget);
			ClearDisguise = reader->Get("ClearDisguise", ClearDisguise);

			ForceMission = reader->Get("ForceMission", ForceMission);

			ExpCost = reader->Get("ExpCost", ExpCost);
			ExpLevel = reader->Get("ExpLevel", ExpLevel);

			ShowMeTheMoney = reader->Get("ShowMeTheMoney", ShowMeTheMoney);
			ShowMeTheMoneyDisplay = reader->Get("ShowMeTheMoneyDisplay", ShowMeTheMoneyDisplay);

			IsToy = reader->Get("IsToy", IsToy);

			Lueluelue = reader->Get("Lueluelue", Lueluelue);

			Teleporter = reader->Get("Teleporter", Teleporter);
			Capturer = reader->Get("Capturer", Capturer);
			IgnoreRevenge = reader->Get("IgnoreRevenge", IgnoreRevenge);
			IgnoreDamageReaction = reader->Get("IgnoreDamageReaction", IgnoreDamageReaction);
			IgnoreDamageReactionModes = reader->GetList("IgnoreDamageReaction.Modes", IgnoreDamageReactionModes);
			if (!IgnoreDamageReactionModes.empty())
			{
				IgnoreDamageReaction = true;
			}
			IgnoreStandShareDamage = reader->Get("IgnoreStandShareDamage", IgnoreStandShareDamage);
		}

		double GetVersus(Armor armor, bool& forceFire, bool& retaliate, bool& passiveAcquire)
		{
			double versus = 1;
			forceFire = true;
			retaliate = true;
			passiveAcquire = true;

			int index = static_cast<int>(armor);
			if (index >= 0)
			{
				int size = AresVersusArray.size();
				if (index < 11)
				{
					// 原始护甲
					versus = Versus[index];
					forceFire = versus > 0.0;
					retaliate = versus > 0.1;
					passiveAcquire = versus > 0.2;
				}
				else
				{
					index -= 11;
					if (index < size)
					{
						// 扩展护甲
						AresVersus aresVersus = AresVersusArray[index];
						versus = aresVersus.Versus;
						forceFire = aresVersus.ForceFire;
						retaliate = aresVersus.Retaliate;
						passiveAcquire = aresVersus.PassiveAcquire;
					}
				}
			}
			return versus;
		}

		static std::string GetArmorName(Armor armor)
		{
			int armorIndex = (int)armor;
			std::string armorName = "unknown";
			if (armorIndex < 11)
			{
				// 默认护甲
				for (auto it : ArmorTypeStrings)
				{
					if (it.second == armor)
					{
						armorName = it.first;
						break;
					}
				}
			}
			else
			{
				// 自定义护甲
				auto array = GetAresArmorArray();
				if ((armorIndex -= 11) < (int)array.size())
				{
					auto it = array.begin();
					if (armorIndex > 0)
					{
						std::advance(it, armorIndex);
					}
					if (it != array.end())
					{
						armorName = it->first;
					}
				}
			}
			return armorName;
		}

	private:
		/**
		 *@brief 读取弹头上的自定义护甲的设置
		 *
		 * @param reader
		 */
		void ReadAresVersus(INIBufferReader* reader)
		{
			if (!GetAresArmorValueArray().empty())
			{
				std::string title = "Versus.";
				for (auto& it : GetAresArmorValueArray())
				{
					// 获得所有自定义护甲的信息
					std::string key = it.first;
					// 护甲类型对应的比例字符串
					std::string value = it.second;

					AresVersus aresVersus;
					// 实际的比例
					if (!value.empty())
					{
						// 比例字符串转成数值
						int armorIndex = -1;
						if (IsDefaultArmor(value, armorIndex) && armorIndex >= 0 && armorIndex < 11)
						{
							double vs = Versus[armorIndex]; // 从弹头比例中读取比例
							if (LESS_EQUAL(vs, 0.02)) {
								aresVersus.PassiveAcquire = false;
							}
							if (LESS_EQUAL(vs, 0.01)) {
								aresVersus.Retaliate = false;
							}
							if (LESS_EQUAL(vs, 0.00)) {
								aresVersus.ForceFire = false;
							}
						}
						else
						{
							// 自定义护甲
							aresVersus.Parse(value.c_str());
						}
					}
					// 读取弹头上的护甲设置
					// 如果是嵌套护甲，没有明写，那么就需要去读嵌套的
					std::stack<std::string> armorNames{};
					GetArmorKeys(key, GetAresArmorArray(), armorNames);
					// 开始读取护甲的设置
					while (!armorNames.empty())
					{
						std::string armor = armorNames.top();
						armorNames.pop();
						aresVersus.Read(reader, title, armor);
					}
					AresVersusArray.emplace_back(aresVersus);
				}
			}
		}
	};

	static int ReadAresArmorTypes()
	{
		return GetAresArmorValueArray().size();
	}

	static constexpr DWORD Canary = 0x22222222;
	// static constexpr size_t ExtPointerOffset = 0x18;

	static WarheadTypeExt::ExtContainer ExtMap;

private:
	/**
	 *@brief 检查是否是游戏默认的护甲，并返回护甲的序号
	 *
	 * @param armor 待检查的护甲
	 * @param index 护甲的序号
	 * @return true
	 * @return false
	 */
	static bool IsDefaultArmor(std::string armor, int& index)
	{
		index = 0;
		std::string a = lowercase(armor);
		auto it = ArmorTypeStrings.find(a);
		if (it != ArmorTypeStrings.end())
		{
			Armor aa = it->second;
			index = static_cast<int>(aa);
			return true;
		}
		return false;
	}

	static void GetArmorKeys(std::string key, std::vector<std::pair<std::string, std::string>> array, std::stack<std::string>& keys)
	{
		for (auto& kv : array)
		{
			if (kv.first == key)
			{
				keys.push(key);
				// 查找是否嵌套
				std::string value = kv.second;
				int index = 0;
				if (value.find("%") != std::string::npos || IsDefaultArmor(value, index))
				{
					return;
				}
				// 迭代查找
				if (value != key)
				{
					GetArmorKeys(value, array, keys);
				}
				break;
			}
		}
	}

	/**
	 *@brief 迭代查找嵌套护甲的最底层的数值
	 *
	 * @param key
	 * @param array
	 * @return std::string
	 */
	static std::string GetArmorValue(std::string key, std::vector<std::pair<std::string, std::string>> array)
	{
		for (auto& kv : array)
		{
			if (kv.first == key)
			{
				std::string value = kv.second;
				// 如果是百分比则返回百分比，如果是游戏默认护甲，则返回默认护甲
				int index = 0;
				if (value.find("%", 0) != std::string::npos || IsDefaultArmor(value, index))
				{
					return value;
				}
				// 迭代查找
				if (value != key)
				{
					return GetArmorValue(value, array);
				}
				break;
			}
		}
		Debug::Log("Warning: Try to read Ares's [ArmorTypes] but type [%s] value is wrong.", key.c_str());
		return "0%";
	}

	/**
	 *@brief 读取Ares护甲注册表，护甲的对应关系.\n
	 * [ArmorTypes]\n
	 * OOXX = 100%\n
	 * XXOO = OOXX\n
	 * @return std::vector<std::pair<std::string, std::string>>
	 */
	static std::vector<std::pair<std::string, std::string>> GetAresArmorArray()
	{
		if (_aresArmorArray.empty())
		{
			const char* section = "ArmorTypes";
			if (INI::HasSection(INI::Rules, section))
			{
				CCINIClass* pINI = CCINIClass::INI_Rules;
				int count = pINI->GetKeyCount(section);
				if (count > 0)
				{
					INIBufferReader* reader = INI::GetSection(INI::Rules, section);
					Debug::Log("Try to read Ares's [ArmorTypes], get %d custom types.\n", count);
					for (int i = 0; i < count; i++)
					{
						std::string keyName = pINI->GetKeyName(section, i);
						std::string value = reader->Get(keyName.c_str(), std::string{ "" });
						if (value.empty())
						{
							Debug::Log("Warning: Try to read Ares's [ArmorTypes], ArmorType %s is %s\n", keyName.c_str(), value.c_str());
							value = "0%";
						}
						_aresArmorArray.push_back(std::pair<std::string, std::string>{ keyName, value });
					}
				}
			}
		}
		return _aresArmorArray;
	}

	/**
	 * @brief 读取整理之后的护甲和比例.\n
	 * [ArmorTypes]\n
	 * OOXX = 100%\n
	 * XXOO = 100%\n
	 *
	 * @return std::vector<std::pair<std::string, std::string>>
	 */
	static std::vector<std::pair<std::string, std::string>> GetAresArmorValueArray()
	{
		if (_aresArmorValueArray.empty() && !GetAresArmorArray().empty())
		{
			// 格式化所有的自定义护甲，包含嵌套护甲，获取实际的护甲信息，作为默认值
			_aresArmorValueArray = { GetAresArmorArray() };
			for (auto& it : GetAresArmorArray())
			{
				std::string key = it.first;
				std::string value = GetArmorValue(key, GetAresArmorArray());
				for (auto& kv : _aresArmorValueArray)
				{
					if (kv.first == key)
					{
						kv.second = value; // 更新为新的值
						break;
					}
				}
			}
			// 输出日志检查是否正确
			std::string logMsg = "[ArmorTypes]\n";
			int i = 11;
			for (auto it : GetAresArmorArray())
			{
				std::string key = it.first;
				std::string value = it.second;
				int nPos = value.find("%", 0);
				int index = 0;
				if (nPos != std::string::npos || IsDefaultArmor(value, index))
				{
					char sp = '%';
					value = nPos >= 0 ? value.insert(nPos, 1, sp) : value;
					logMsg += "     Armor " + std::to_string(i) + " - " + key + " = " + value + "\n";
				}
				else
				{
					std::string vv = "";
					for (auto& kv : _aresArmorValueArray)
					{
						if (kv.first == key)
						{
							vv = kv.second;
							break;
						}
					}
					nPos = vv.find("%", 0);
					char sp = '%';
					vv = nPos >= 0 ? vv.insert(nPos, 1, sp) : vv;
					logMsg += "     Armor " + std::to_string(i) + " - " + key + " = " + value + " = " + vv + "\n";
				}
				i++;
			}
			Debug::Log(logMsg.c_str());
		}
		return _aresArmorValueArray;
	}

	inline static std::vector<std::pair<std::string, std::string>> _aresArmorArray{}; // Ares护甲注册表，护甲的对应关系
	inline static std::vector<std::pair<std::string, std::string>> _aresArmorValueArray{}; // 读取完所有嵌套护甲对应的百分比默认值


};

static bool HasPreImpactAnim(WarheadTypeClass* pWH)
{
	return IsNotNone(GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH)->PreImpactAnim);
}
