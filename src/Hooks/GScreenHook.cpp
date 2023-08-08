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
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, Kratos::DrawVersionText);
	}
};

static GScreenHook _GScreenHook;

DEFINE_HOOK(0x4F4497, GScreenClass_Render, 0x6)
{
	EventSystems::Render.Broadcast(Events::GScreenRenderEvent, nullptr);
	return 0;
}

DEFINE_HOOK(0x4F4583, GScreenClass_Render_Late, 0x6)
{
	bool late = true;
	EventSystems::Render.Broadcast(Events::GScreenRenderEvent, &late);
	return 0;
}

/*
DEFINE_HOOK(0x6A70AE, SidebarClass_Draw_It, 0x5)
{
// Draw Version Text
RectangleStruct textRect = Drawing::GetTextDimensions(Kratos::VersionDescription, { 0, 0 }, 0, 2, 0);
RectangleStruct sidebarRect = DSurface::Sidebar->GetRect();
int x = sidebarRect.Width / 2 - textRect.Width / 2;
int y = sidebarRect.Height - textRect.Height;
Point2D pos = { x, y };

DSurface::Sidebar->DrawText(Kratos::VersionDescription, &pos, COLOR_RED);

return 0;
}
*/
