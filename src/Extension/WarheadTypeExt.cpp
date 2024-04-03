#include "WarheadTypeExt.h"

bool WarheadTypeExt::IsDefaultArmor(std::string armor, int& index)
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

std::string WarheadTypeExt::GetArmorValue(std::string key, std::vector<std::pair<std::string, std::string>> array)
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
	Debug::Log("Warning: Try to read Ares's [ArmorTypes] but type [%s] value is wrong.\n", key.c_str());
	return "0%";
}


std::vector<std::pair<std::string, std::string>> WarheadTypeExt::GetAresArmorArray()
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

std::vector<std::pair<std::string, AresVersus>> WarheadTypeExt::GetAresArmorValueArray()
{
	if (_aresArmorValueArray.empty() && !GetAresArmorArray().empty())
	{
		// 格式化所有的自定义护甲，包含嵌套护甲，获取实际的护甲信息，作为默认值
		std::vector<std::pair<std::string, std::string>> armorArray = GetAresArmorArray();
		std::vector<std::pair<std::string, std::string>> tempArray = { armorArray };
		for (auto it : armorArray)
		{
			std::string key = it.first;
			std::string value = GetArmorValue(key, armorArray);
			for (auto& kv : tempArray)
			{
				if (kv.first == key)
				{
					kv.second = value; // 更新为新的值
					break;
				}
			}
		}
		// 读取护甲的具体数值
		for (auto tempKV : tempArray)
		{
			std::string k = tempKV.first;
			std::string v = tempKV.second;
			int index = 0;
			AresVersus aresVersus;
			if (!IsDefaultArmor(v, index))
			{
				aresVersus.Parse(v.c_str());
			}
			_aresArmorValueArray.push_back(std::pair<std::string, AresVersus>{ k, aresVersus });
		}
		// 输出日志检查是否正确
		std::string logMsg = "[ArmorTypes]\n";
		Debug::LogNotTitle(logMsg.c_str());
		int i = 11;
		for (auto it : armorArray)
		{
			std::string key = it.first;
			std::string value = it.second;
			int nPos = value.find("%", 0);
			int index = 0;
			if (nPos != std::string::npos || IsDefaultArmor(value, index))
			{
				char sp = '%';
				value = nPos >= 0 ? value.insert(nPos, 1, sp) : value;
				logMsg = "  " + std::to_string(i) + " - " + key + " = " + value + "\n";
			}
			else
			{
				std::string vv = "";
				for (auto& kv : tempArray)
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
				logMsg = "  " + std::to_string(i) + " - " + key + " = " + value + " = " + vv + "\n";
			}
			Debug::LogNotTitle(logMsg.c_str());
			i++;
		}
	}
	return _aresArmorValueArray;
}

std::vector<std::pair<std::string, std::string>> WarheadTypeExt::_aresArmorArray;
std::vector<std::pair<std::string, AresVersus>> WarheadTypeExt::_aresArmorValueArray;

WarheadTypeExt::ExtContainer WarheadTypeExt::ExtMap;
