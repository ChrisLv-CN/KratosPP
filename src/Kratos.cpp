#include <Helpers/Macro.h>

#include <CCINIClass.h>
#include <Unsorted.h>
#include <Drawing.h>
#include <HouseClass.h>
#include <MessageListClass.h>

#include <exception>
#include <string>

#include <Kratos.h>

const wchar_t* Kratos::messageLabel = L"Kratos";
const wchar_t* Kratos::versionDescription = L"KratosPP Ver." VERSION_SHORT_WSTR;

void Kratos::SendActiveMessage(EventSystem* sender, Event e, void* args)
{
	if (args)
	{
		std::wstring v = VERSION_SHORT_WSTR;
		std::wstring msg = L"Version " + v + L" is active, have fun.";

		MessageListClass::Instance->PrintMessage(Kratos::messageLabel, msg.c_str(), 150, HouseClass::CurrentPlayer->ColorSchemeIndex, true);
		sender->RemoveHandler(e, SendActiveMessage);
	}
}

void Kratos::DrawVersionText(EventSystem* sender, Event e, void* args)
{
	// Draw Version Text
	RectangleStruct textRect = Drawing::GetTextDimensions(Kratos::versionDescription, { 0, 0 }, 0, 2, 0);
	RectangleStruct sidebarRect = DSurface::Sidebar->GetRect();
	int x = sidebarRect.Width / 2 - textRect.Width / 2;
	int y = sidebarRect.Height - textRect.Height;
	Point2D pos = { x, y };

	DSurface::Sidebar->DrawText(Kratos::versionDescription, &pos, Drawing::RGB_To_Int(Drawing::TooltipColor));
}
