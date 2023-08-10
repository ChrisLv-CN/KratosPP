#pragma once
#include <Windows.h>
#include <Version.h>

#include <Common/EventSystems/EventSystem.h>

class Kratos
{
public:

	static const wchar_t* messageLabel;

	static const wchar_t* versionShot;
	static const wchar_t* versionDescription;

	static void SendActiveMessage(EventSystem* sender, Event e, void* args);
	static void DrawVersionText(EventSystem* sender, Event e, void* args);

};