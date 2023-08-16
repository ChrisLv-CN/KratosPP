#pragma once
#pragma comment( lib, "KratosPPLib.lib" )
#include <Windows.h>
#include <Version.h>

#include <GeneralStructures.h>
#include <Timer.h>
#include <VoxClass.h>
#include <VocClass.h>
#include <Common.h>
#include <Common/EventSystems/EventSystem.h>

#include <KratosLib.h>

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

	static void ExeRun(EventSystem* sender, Event e, void* args);

	static void SendActiveMessage(EventSystem* sender, Event e, void* args);
	static void DrawVersionText(EventSystem* sender, Event e, void* args);
};
