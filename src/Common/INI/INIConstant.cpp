#include "INIConstant.h"

std::string INIConstant::_rulesName{};
std::string INIConstant::_artName{};
std::string INIConstant::_aiName{};

std::string INIConstant::_ra2md = "ra2md.ini";

std::string INIConstant::_gameModeName{};
std::string INIConstant::_mapName{};

std::string_view INIConstant::GetRulesName()
{
	if (_rulesName.empty())
	{
		_rulesName = (LPCSTR)CCINIClass::INI_Rules_FileName.get();
	}
	return _rulesName;
}

std::string_view INIConstant::GetArtName()
{
	if (_artName.empty())
	{
		_artName = (LPCSTR)CCINIClass::INI_Art_FileName.get();
	}
	return _artName;
}

std::string_view INIConstant::GetAIName()
{
	if (_aiName.empty())
	{
		_aiName = (LPCSTR)CCINIClass::INI_AI_FileName.get();
	}
	return _aiName;
}

std::string_view INIConstant::GetRa2md()
{
	return _ra2md;
}

std::string_view INIConstant::GetGameModeName()
{
	return _gameModeName;
}

std::string_view INIConstant::GetMapName()
{
	if (_mapName.empty())
	{
		_mapName = ScenarioClass::Instance->FileName;
	}
	return _mapName;
}

void INIConstant::SetGameModeName(EventSystem* sender, Event e, void* args)
{
	_gameModeName = SessionClass::Instance->MPGameMode->INIFilename.Buffer;
#ifdef DEBUG
	Debug::Log("Config file info:\n  Rules = \"%s\"\n  Art = \"%s\"\n  Ai = \"%s\"\n  MapName = \"%s\"\n  GameMode = \"%s\"\n",
		GetRulesName().data(),
		GetArtName().data(),
		GetAIName().data(),
		GetMapName().data(),
		GetGameModeName().data()
	);
#endif // DEBUG
}

void INIConstant::SaveGameModeName(EventSystem* sender, Event e, void* args)
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

void INIConstant::LoadGameModeName(EventSystem* sender, Event e, void* args)
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