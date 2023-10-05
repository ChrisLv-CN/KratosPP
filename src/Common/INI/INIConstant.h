#pragma once

#include <string>

#include <CCINIClass.h>
#include <SessionClass.h>
#include <ScenarioClass.h>

#include <Utilities/Debug.h>
#include <Utilities/Stream.h>
#include <Utilities/SavegameDef.h>

#include <Common/EventSystems/EventSystem.h>

namespace INIConstant
{
	static std::string _rulesName{}; // rulesmd.ini
	static std::string _artName{};   // artmd.ini
	static std::string _aiName{};	 // aimd.ini

	static std::string _ra2md = "ra2md.ini"; // ra2md.ini

	static std::string _gameModeName{};
	static std::string _mapName{};

	static std::string_view GetRulesName()
	{
		if (_rulesName.empty())
		{
			_rulesName = (LPCSTR)CCINIClass::INI_Rules_FileName.get();
		}
		return _rulesName;
	}

	static std::string_view GetArtName()
	{
		if (_artName.empty())
		{
			_artName = (LPCSTR)CCINIClass::INI_Art_FileName.get();
		}
		return _artName;
	}

	static std::string_view GetAIName()
	{
		if (_aiName.empty())
		{
			_aiName = (LPCSTR)CCINIClass::INI_AI_FileName.get();
		}
		return _aiName;
	}

	static std::string_view GetRa2md()
	{
		return _ra2md;
	}

	static std::string_view GetGameModeName()
	{
		return _gameModeName;
	}

	static std::string_view GetMapName()
	{
		if (_mapName.empty())
		{
			_mapName = ScenarioClass::Instance->FileName;
		}
		return _mapName;
	}

	static void SetGameModeName(EventSystem* sender, Event e, void* args)
	{
		// 直接进战役为空指针
		MPGameModeClass* pMPGame = SessionClass::Instance->MPGameMode;
		if (pMPGame)
		{
			_gameModeName = pMPGame->INIFilename.Buffer;
		}
#ifdef DEBUG
		Debug::Log("Config file info:\n  Rules = \"%s\"\n  Art = \"%s\"\n  Ai = \"%s\"\n  MapName = \"%s\"\n  GameMode = \"%s\"\n",
			GetRulesName().data(),
			GetArtName().data(),
			GetAIName().data(),
			GetMapName().data(),
			GetGameModeName().data());
#endif // DEBUG
	}

#pragma region Save/Load
	static void SaveGameModeName(EventSystem* sender, Event e, void* args)
	{
		SaveGameEventArgs* arg = (SaveGameEventArgs*)args;
		if (arg->IsStartInStream())
		{
			ExByteStream saver(_gameModeName.size());
			ExStreamWriter writer(saver);
			writer.Process(_gameModeName, false);
			saver.WriteBlockToStream(arg->Stream);
		}
	}
	static void LoadGameModeName(EventSystem* sender, Event e, void* args)
	{
		LoadGameEventArgs* arg = (LoadGameEventArgs*)args;
		if (arg->IsStartInStream())
		{
			ExByteStream loader(0);
			loader.ReadBlockFromStream(arg->Stream);
			ExStreamReader reader(loader);
			reader.Process(_gameModeName, false);
		}
	}
#pragma endregion

};
