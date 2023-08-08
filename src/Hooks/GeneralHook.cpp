#include <Helpers/Macro.h>

#include <exception>

#include <Common/EventSystems/EventSystem.h>
#include <Windows.h>

class GeneraHook
{
public:
	GeneraHook()
	{
	}
};

static GeneraHook _GeneraHook;

DEFINE_HOOK(0x52BA60, YR_Boot, 0x5)
{
    return 0;
}

DEFINE_HOOK(0x7CD810, ExeRun, 0x9)
{
#ifdef DEBUG
    MessageBoxW(NULL,
        L"You can now attach a debugger.\n\n"

        L"To attach a debugger find the YR process in Process Hacker "
        L"/ Visual Studio processes window and detach debuggers from it, "
        L"then you can attach your own debugger. After this you should "
        L"terminate Syringe.exe because it won't automatically exit when YR is closed.\n\n"

        L"Press OK to continue YR execution.",
        L"Debugger Notice", MB_OK);
#endif
    return 0;
}

DEFINE_HOOK(0x7CD8EF, ExeTerminate, 0x9)
{
    return 0;
}

DEFINE_HOOK(0x685659, Scenario_ClearClasses, 0xA)
{
    EventSystems::General.Broadcast(Events::ScenarioClearClassesEvent, nullptr);
    return 0;
}

// in progress: Initializing Tactical display
DEFINE_HOOK(0x6875F3, Scenario_Start1, 0x6)
{
    EventSystems::General.Broadcast(Events::ScenarioStartEvent, nullptr);
    return 0;
}


DEFINE_HOOK(0x55AFB3, LogicClass_Update, 0x6)
{
	EventSystems::Logic.Broadcast(Events::LogicUpdateEvent, nullptr);
	return 0;
}

DEFINE_HOOK(0x55B719, LogicClass_Update_Late, 0x5)
{
	bool late = true;
	EventSystems::Logic.Broadcast(Events::LogicUpdateEvent, &late);
	return 0;
}
