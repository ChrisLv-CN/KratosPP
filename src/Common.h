#pragma once
#include <Windows.h>

#include <Version.h>
#include <Common/EventSystems/EventSystem.h>

#define CAN_USE_ARES 1

constexpr auto NONE_STR = "<none>";
constexpr auto NONE_STR2 = "none";
constexpr auto TOOLTIPS_SECTION = "ToolTips";
constexpr auto SIDEBAR_SECTION = "Sidebar";

extern bool InChinese;

class EventSystem;
class Event;

class Common
{
public:
	//variables
	static HANDLE hInstance;

	static bool IsLoadGame;
	static bool IsScenarioClear;

	static bool DebugAE;

	static const size_t readLength = 2048;
	static char readBuffer[readLength];
	static wchar_t wideBuffer[readLength];
	static const char readDelims[4];

	static void CmdLineParse(EventSystem* sender, Event e, void* args);

	static void ExeRun(EventSystem* sender, Event e, void* args);
	static void ExeTerminate(EventSystem* sender, Event e, void* args);

#ifdef DEBUG
	static bool DetachFromDebugger();
#endif // DEBUG

};
