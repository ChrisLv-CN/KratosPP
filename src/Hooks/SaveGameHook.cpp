#include <Helpers/Macro.h>

#include <exception>
#include <string>

#include <Common.h>

#include <Common/EventSystems/EventSystem.h>

#include <Extension/TechnoExt.h>

#include <Ext/Common/FireSuperManager.h>

class SaveGameHook
{
public:
	SaveGameHook()
	{
		EventSystems::SaveLoad.AddHandler(Events::SaveGameEvent, INIConstant::SaveGameModeName);
		EventSystems::SaveLoad.AddHandler(Events::LoadGameEvent, INIConstant::LoadGameModeName);
		EventSystems::SaveLoad.AddHandler(Events::SaveGameEvent, FireSuperManager::SaveSuperQueue);
		EventSystems::SaveLoad.AddHandler(Events::LoadGameEvent, FireSuperManager::LoadSuperQueue);
		EventSystems::SaveLoad.AddHandler(Events::LoadGameEvent, TechnoExt::ClearBaseArray);
	}
};

static SaveGameHook _saveGameHook;

// Loading a save game skip the Hook like TechnoClass_CTOR to Allocate a new TechnoExt
bool IsLoadGame = false;

DEFINE_HOOK(0x67CEF0, SaveGame_Start, 0x6)
{
	GET(const char *, fileName, ECX);
	// GET(const char*, scenarioDescription, EDX);
	SaveGameEventArgs args{fileName, true};
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67D2F1, SaveGame_End, 0x6)
{
	GET(const char *, fileName, EDI);
	// GET(const char*, scenarioDescription, ESI);
	SaveGameEventArgs args{fileName, false};
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67D300, SaveGameInStream_Start, 0x5)
{
	GET(IStream *, stream, ECX);
	SaveGameEventArgs args{stream, true};
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67E42E, SaveGameInStream_End, 0xD)
{
	GET(IStream *, stream, ESI);
	SaveGameEventArgs args{stream, false};
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67E440, LoadGame_Start, 0x6)
{
	GET(const char *, fileName, ECX);
	LoadGameEventArgs args{fileName, true};
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67E720, LoadGame_End, 0x6)
{
	GET(const char *, fileName, ESI);
	LoadGameEventArgs args{fileName, false};
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67E730, LoadGameInStream_Start, 0x5)
{
	IsLoadGame = true;
	GET(IStream *, stream, ECX);
	LoadGameEventArgs args{stream, true};
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, &args);
	return 0;
}

DEFINE_HOOK(0x67F7C8, LoadGameInStream_End, 0x5)
{
	IsLoadGame = false;
	GET(IStream *, stream, ESI);
	LoadGameEventArgs args{stream, false};
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, &args);
	return 0;
}
