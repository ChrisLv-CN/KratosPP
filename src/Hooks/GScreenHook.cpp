#include <Helpers/Macro.h>

#include <exception>

#include <Common/EventSystems/EventSystem.h>
#include <Ext/PrintTextManager.h>

class GScreenHook
{
public:
	GScreenHook()
	{
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, PrintTextManager::PrintRollingText);
	}
};

static GScreenHook _gScreenHook;

DEFINE_HOOK(0x4F4497, GScreenClass_Render, 0x6)
{
	EventSystems::Render.Broadcast(Events::GScreenRenderEvent);
	return 0;
}

DEFINE_HOOK(0x4F4583, GScreenClass_Render_Late, 0x6)
{
	EventSystems::Render.Broadcast(Events::GScreenRenderEvent, EventArgsLate);
	return 0;
}

// SidebarClass_5F38C0
DEFINE_HOOK(0x6A70EB, SidebarClass_DrawIt, 0x6)
{
	EventSystems::Render.Broadcast(Events::SidebarRenderEvent);
	return 0;
}
