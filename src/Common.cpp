#include <Common.h>

#include <Drawing.h>

#include <Utilities/Debug.h>
#include <Utilities/Patch.h>
#include <Utilities/Macro.h>

#include <Utilities/AresHelper.h>

#ifndef IS_RELEASE_VER
bool HideWarning = false;
#endif

bool InChinese = GetSystemDefaultLangID() == 0x0804;

HANDLE Common::hInstance = 0;

char Common::readBuffer[Common::readLength];
wchar_t Common::wideBuffer[Common::readLength];
const char Common::readDelims[4] = ",";

const char* Common::AppIconPath = nullptr;

//void Common::CmdLineParse(char** ppArgs, int nNumArgs)
void Common::CmdLineParse(EventSystem* sender, Event e, void* args)
{
	bool foundInheritance = false;
	bool foundInclude = false;

	auto const& argsArray = reinterpret_cast<void**>(args);
	char** ppArgs = (char**)argsArray[0];
	int nNumArgs = (int)argsArray[1];

	// > 1 because the exe path itself counts as an argument, too!
	for (int i = 1; i < nNumArgs; i++)
	{
		const char* pArg = ppArgs[i];

		if (_stricmp(pArg, "-Icon") == 0)
		{
			Common::AppIconPath = ppArgs[++i];
		}
#ifndef IS_RELEASE_VER
		if (_stricmp(pArg, "-b=" STR(BUILD_NUMBER)) == 0)
		{
			HideWarning = true;
		}
#endif
		if (_stricmp(pArg, "-Inheritance") == 0)
		{
			foundInheritance = true;
		}
		if (_stricmp(pArg, "-Include") == 0)
		{
			foundInclude = true;
		}
	}

	if (foundInclude)
	{
		Patch::Apply_RAW(0x474200, // Apply CCINIClass_ReadCCFile1_DisableAres
			{ 0x8B, 0xF1, 0x8D, 0x54, 0x24, 0x0C }
		);

		Patch::Apply_RAW(0x474314, // Apply CCINIClass_ReadCCFile2_DisableAres
			{ 0x81, 0xC4, 0xA8, 0x00, 0x00, 0x00 }
		);
	}
	else
	{
		Patch::Apply_RAW(0x474230, // Revert CCINIClass_Load_Inheritance
			{ 0x8B, 0xE8, 0x88, 0x5E, 0x40 }
		);
	}

	if (foundInheritance)
	{
		Patch::Apply_RAW(0x528A10, // Apply INIClass_GetString_DisableAres
			{ 0x83, 0xEC, 0x0C, 0x33, 0xC0 }
		);

		Patch::Apply_RAW(0x526CC0, // Apply INIClass_GetKeyName_DisableAres
			{ 0x8B, 0x54, 0x24, 0x04, 0x83, 0xEC, 0x0C }
		);
	}
	else
	{
		Patch::Apply_RAW(0x528BAC, // Revert INIClass_GetString_Inheritance_NoEntry
			{ 0x8B, 0x7C, 0x24, 0x2C, 0x33, 0xC0, 0x8B, 0x4C, 0x24, 0x28 }
		);
	}

	if (InChinese)
	{
		//Debug::Log("初始化完成 版本: " PRODUCT_VERSION "\n");
	}
	else
	{
		//Debug::Log("Initialized version: " PRODUCT_VERSION "\n");
	}
}

void Common::ExeRun(EventSystem* sender, Event e, void* args)
{
	Patch::ApplyStatic();

#ifdef DEBUG
	if (InChinese)
	{
		if (Common::DetachFromDebugger())
		{
			MessageBoxW(NULL,
				L"你可以去附加调试器了。\n\n"

				L"按下OK继续运行尤里的复仇。",
				L"调试信息", MB_OK);
		}
		else
		{
			MessageBoxW(NULL,
				L"你可以去附加调试器了。\n\n"

				L"To attach a debugger find the YR process in Process Hacker "
				L"/ Visual Studio processes window and detach debuggers from it, "
				L"then you can attach your own debugger. After this you should "
				L"terminate Syringe.exe because it won't automatically exit when YR is closed.\n\n"

				L"按下OK继续运行尤里的复仇。",
				L"调试信息", MB_OK);
		}
	}
	else
	{
		if (Common::DetachFromDebugger())
		{
			MessageBoxW(NULL,
				L"You can now attach a debugger.\n\n"

				L"Press OK to continue YR execution.",
				L"Debugger Notice", MB_OK);
		}
		else
		{
			MessageBoxW(NULL,
				L"You can now attach a debugger.\n\n"

				L"To attach a debugger find the YR process in Process Hacker "
				L"/ Visual Studio processes window and detach debuggers from it, "
				L"then you can attach your own debugger. After this you should "
				L"terminate Syringe.exe because it won't automatically exit when YR is closed.\n\n"

				L"Press OK to continue YR execution.",
				L"Debugger Notice", MB_OK);
		}
	}

	if (!Console::Create())
	{
		if (InChinese)
		{
			MessageBoxW(NULL,
				L"控制台无法打开！",
				L"调试信息", MB_OK);
		}
		else
		{
			MessageBoxW(NULL,
				L"Failed to allocate the debug console!",
				L"Debug Console Notice", MB_OK);
		}
	}

#endif
	AresHelper::Init();
}

void Common::ExeTerminate(EventSystem* sender, Event e, void* args)
{
	Console::Release();
}

// =============================
// hooks

bool __stdcall DllMain(HANDLE hInstance, DWORD dwReason, LPVOID v)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Common::hInstance = hInstance;
	}
	return true;
}
