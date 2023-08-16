#pragma once

#include <Common/EventSystems/EventSystem.h>

class KratosLib
{
public:
	static void Load();
	static void CmdLineParse(EventSystem* sender, Event e, void* args);
};


