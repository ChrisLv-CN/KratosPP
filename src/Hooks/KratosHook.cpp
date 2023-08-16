#include <Common/EventSystems/EventSystem.h>
#include <Kratos.h>

class KratosHook
{
public:
	KratosHook()
	{
		EventSystems::General.AddHandler(Events::ExeRun, Kratos::ExeRun);

		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, Kratos::SendActiveMessage);
		EventSystems::Render.AddHandler(Events::SidebarRenderEvent, Kratos::DrawVersionText);
	}
};

static KratosHook _KratosHook;

