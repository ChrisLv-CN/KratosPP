
#include <Kratos.h>
#include <Kratos.version.h>

#include <Helpers/Macro.h>

#include <CCINIClass.h>
#include <Unsorted.h>
#include <Drawing.h>

#include <exception>

HANDLE Kratos::hInstance = 0;

const wchar_t* Kratos::VersionDescription = L"KratosPP Ver." str(BUILD_NUMBER);

bool __stdcall DllMain(HANDLE hInstance, DWORD dwReason, LPVOID v)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Kratos::hInstance = hInstance;
	}
	return true;
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

DEFINE_HOOK(0x6A70AE, SidebarClass_Draw_It, 0x5)
{
	/*
	// Draw Version Text
	RectangleStruct textRect = Drawing::GetTextDimensions(Kratos::VersionDescription, { 0, 0 }, 0, 2, 0);
	RectangleStruct sidebarRect = DSurface::Sidebar->GetRect();
	int x = sidebarRect.Width / 2 - textRect.Width / 2;
	int y = sidebarRect.Height - textRect.Height;
	Point2D pos = { x, y };

	DSurface::Sidebar->DrawText(Kratos::VersionDescription, &pos, COLOR_RED);
	*/
	return 0;
}
