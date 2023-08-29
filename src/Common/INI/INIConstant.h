#pragma once

#include <string>

#include <CCINIClass.h>
#include <SessionClass.h>
#include <ScenarioClass.h>

#include <Utilities/Debug.h>
#include <Utilities/Stream.h>
#include <Utilities/SavegameDef.h>
#include <Common/EventSystems/EventSystem.h>

class INIConstant
{
public:
	static void SetGameModeName(EventSystem* sender, Event e, void* args);
	static void SaveGameModeName(EventSystem* sender, Event e, void* args);
	static void LoadGameModeName(EventSystem* sender, Event e, void* args);

	static std::string_view GetRulesName();
	static std::string_view GetArtName();
	static std::string_view GetAIName();

	static std::string_view GetRa2md();

	static std::string_view GetGameModeName();
	static std::string_view GetMapName();
private:
	static std::string _rulesName; // rulesmd.ini
	static std::string _artName; // artmd.ini
	static std::string _aiName; // aimd.ini

	static std::string _ra2md; // ra2md.ini

	static std::string _gameModeName;
	static std::string _mapName;
};


