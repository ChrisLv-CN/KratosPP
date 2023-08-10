#include <Helpers/Macro.h>

#include <exception>

#include <Common/EventSystems/EventSystem.h>
#include <Kratos.h>

class GScreenHook
{
public:
	GScreenHook()
	{
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, Kratos::SendActiveMessage);
		EventSystems::Render.AddHandler(Events::SidebarRenderEvent, Kratos::DrawVersionText);
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, Kratos::DrawNotAllowed);
		EventSystems::Logic.AddHandler(Events::LogicUpdateEvent, Kratos::HappyMode);
	}
};

static GScreenHook _GScreenHook;

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

DEFINE_HOOK(0x6A6EB1, SidebarClass_DrawIt, 0x6)
{
	EventSystems::Render.Broadcast(Events::SidebarRenderEvent);
	return 0;
}
