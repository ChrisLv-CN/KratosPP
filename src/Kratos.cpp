#include <Helpers/Macro.h>

#include <CCINIClass.h>
#include <Unsorted.h>
#include <Drawing.h>
#include <HouseClass.h>
#include <MessageListClass.h>

#include <exception>

#include <Kratos.h>
#include <Kratos.version.h>

HANDLE Kratos::hInstance = 0;

const wchar_t* Kratos::MessageLabel = L"Kratos";
const wchar_t* Kratos::VersionDescription = L"KratosPP Ver." str(FILE_VERSION_STR);


void Kratos::SendActiveMessage(EventSystem* sender, Event e, void* args)
{
	if (args)
	{
		const wchar_t* message = L"Version " FILE_VERSION_STR L" is active, have fun.";
		MessageListClass::Instance->PrintMessage(Kratos::MessageLabel, message, 150, HouseClass::CurrentPlayer->ColorSchemeIndex, true);
		sender->RemoveHandler(e, SendActiveMessage);
	}
}

void Kratos::DrawVersionText(EventSystem* sender, Event e, void* args)
{
	// Draw Version Text
	RectangleStruct textRect = Drawing::GetTextDimensions(Kratos::VersionDescription, { 0, 0 }, 0, 2, 0);
	RectangleStruct sidebarRect = DSurface::Sidebar->GetRect();
	int x = sidebarRect.Width / 2 - textRect.Width / 2;
	int y = sidebarRect.Height - textRect.Height;
	Point2D pos = { x, y };

	DSurface::Sidebar->DrawText(Kratos::VersionDescription, &pos, Drawing::RGB_To_Int(Drawing::TooltipColor));
}

bool __stdcall DllMain(HANDLE hInstance, DWORD dwReason, LPVOID v)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Kratos::hInstance = hInstance;
	}
	return true;
}
