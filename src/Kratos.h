#pragma once
#include <Windows.h>
#include <Version.h>

#include <GeneralStructures.h>
#include <Timer.h>
#include <VoxClass.h>
#include <VocClass.h>
#include <Common.h>
#include <Common/EventSystems/EventSystem.h>
#include <Misc/HappyMode.h>



class Kratos
{
public:
	struct VersionText
	{
		const wchar_t* text;
		Point2D pos;
		int color;
	};

	static VersionText _versionText;

	static bool _disableHappyMode;
	static CDTimerClass _happyModeDelay;
	static int _happyModeMessageIndex;

	static void SendActiveMessage(EventSystem* sender, Event e, void* args);
	static void DrawVersionText(EventSystem* sender, Event e, void* args);

	static void DrawNotAllowed(EventSystem* sender, Event e, void* args);
	static void HappyMode(EventSystem* sender, Event e, void* args);

	static bool EnableHappyMode();
};
