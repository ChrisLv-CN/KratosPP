#include <Helpers/Macro.h>

#include <exception>

#include <Common/EventSystems/EventSystem.h>
#include <Common.h>

/*
* 直接使用Phobos的保存读取，废弃该事件

DEFINE_HOOK(0x67CEF0, SaveGame_Start, 0x6)
{
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, nullptr);
	return 0;
}

DEFINE_HOOK(0x67D2F1, SaveGame_End, 0x6)
{
	bool late = true;
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, &late);
	return 0;
}

DEFINE_HOOK(0x67D300, SaveGameInStream_Start, 0x5)
{
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, nullptr);
	return 0;
}

DEFINE_HOOK(0x67E42E, SaveGameInStream_End, 0xD)
{
	bool late = true;
	EventSystems::SaveLoad.Broadcast(Events::SaveGameEvent, &late);
	return 0;
}

DEFINE_HOOK(0x67E440, LoadGame_Start, 0x6)
{
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, nullptr);
	return 0;
}

DEFINE_HOOK(0x67E720, LoadGame_End, 0x6)
{
	bool late = true;
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, &late);
	return 0;
}

DEFINE_HOOK(0x67E730, LoadGameInStream_Start, 0x5)
{
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, nullptr);
	return 0;
}

DEFINE_HOOK(0x67F7C8, LoadGameInStream_End, 0x5)
{
	bool late = true;
	EventSystems::SaveLoad.Broadcast(Events::LoadGameEvent, &late);
	return 0;
}

*/
