#pragma once

#include <ArrayClasses.h>
#include <GeneralDefinitions.h>
#include <Helpers/CompileTime.h>

struct TacticalSelectableStruct;
class SideClass;
class ObjectClass;

// things that I can't put into nice meaningful classes
class Game
{
public:
	// the magic checksum for version validation - linked in StaticInits
	static constexpr reference<DWORD, 0x83D560u> const Savegame_Magic{};

	static constexpr reference<HWND, 0xB73550u> const hWnd{};
	static constexpr reference<HINSTANCE, 0xB732F0u> const hInstance{};

	static constexpr reference<bool, 0x840A6Cu> const bVideoBackBuffer{};
	static constexpr reference<bool, 0xA8EB96u> const bAllowVRAMSidebar{};

	// the game's own rounding function
	// infamous for true'ing (F2I(-5.00) == -4.00)
	static __int64 F2I64(double val) {
#ifdef _MSVC
		double something = val;
		__asm { fld something };
		CALL(0x7C5F00);
#endif
	}

	// the game's own rounding function
	// infamous for true'ing (F2I(-5.00) == -4.00)
	static int F2I(double val) {
		return static_cast<int>(F2I64(val));
	}

	[[noreturn]] static void RaiseError(HRESULT err)
		{ JMP_STD(0x7DC720); }

	// actually is SessionClass::Callback
	static void SetProgress(int progress)
		{ SET_REG32(ECX, 0xA8B238); JMP_STD(0x69AE90); }

	static void CallBack()
		{ JMP_STD(0x48D080); }

	static int __fastcall GetResource(int ID, int Type)
		{ JMP_STD(0x4A3B40); }

	static void __fastcall CenterWindowIn(HWND Child, HWND Parent)
		{ JMP_STD(0x777080); }

	static void __fastcall sub_53E420(HWND hWindow)
		{ JMP_STD(0x53E420); }

	static void __fastcall sub_53E3C0(HWND hWindow)
		{ JMP_STD(0x53E3C0); }

	static void __stdcall OnWindowMoving(tagRECT *Rect)
		{ JMP_STD(0x776D80); }

	static void __stdcall PlanningManager_WM_RBUTTONUP_63AB00(Point2D XY)
		{ JMP_STD(0x63AB00); }

	static HRESULT __fastcall Save_Sides(LPSTREAM pStm, DynamicVectorClass<SideClass *>* pVector)
		{ JMP_STD(0x6805F0); }

	static void StreamerThreadFlush()
		{ JMP_STD(0x53E6B0); }

	static void __fastcall UICommands_TypeSelect_7327D0(const char* iniName)
		{ JMP_STD(0x7327D0); }

	static bool IsTypeSelecting()
		{ JMP_STD(0x732D00); }

	static double GetFloaterGravity()
		{ JMP_STD(0x48ACF0); }
};

// this fake class contains the IIDs used by the game
// no comments because the IIDs suck
class IIDs {
public:
	static constexpr reference<IID const, 0x7F7C90u> const IUnknown{};
	static constexpr reference<IID const, 0x7F7C80u> const IPersistStream{};
	static constexpr reference<IID const, 0x7F7C70u> const IPersist{};
	static constexpr reference<IID const, 0x7E9AE0u> const IRTTITypeInfo{};
	static constexpr reference<IID const, 0x7EA768u> const IHouse{};
	static constexpr reference<IID const, 0x7E9B00u> const IPublicHouse{};
	static constexpr reference<IID const, 0x7F7CB0u> const IEnumConnections{};
	static constexpr reference<IID const, 0x7F7CC0u> const IConnectionPoint{};
	static constexpr reference<IID const, 0x7F7CD0u> const IConnectionPointContainer{};
	static constexpr reference<IID const, 0x7F7CE0u> const IEnumConnectionPoints{};
	static constexpr reference<IID const, 0x7E36C0u> const IApplication{};
	static constexpr reference<IID const, 0x7EA6E8u> const IGameMap{};
	static constexpr reference<IID const, 0x7ED358u> const ILocomotion{};
	static constexpr reference<IID const, 0x7E9B10u> const IPiggyback{};
	static constexpr reference<IID const, 0x7E9B40u> const IFlyControl{};
	static constexpr reference<IID const, 0x7E9B20u> const ISwizzle{};
};

// this class links to functions gamemd imports
// to avoid having to link to their DLLs ourselves
class Imports {
public:
	// OleLoadFromStream
	typedef HRESULT (__stdcall * FP_OleLoadFromStream)(LPSTREAM pStm, const IID *const iidInterface, LPVOID *ppvObj);
	static FP_OleLoadFromStream &OleLoadFromStream;

	typedef DWORD (* FP_TimeGetTime)();
	static FP_TimeGetTime &TimeGetTime;

	/* user32.dll */
	typedef LRESULT ( __stdcall * FP_DefWindowProcA)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static FP_DefWindowProcA &DefWindowProcA;

	typedef BOOL ( __stdcall * FP_MoveWindow)(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
	static FP_MoveWindow &MoveWindow;

	typedef BOOL ( __stdcall * FP_GetUpdateRect)(HWND hWnd, LPRECT lpRect, BOOL bErase);
	static FP_GetUpdateRect &GetUpdateRect;

	typedef HWND ( * FP_GetFocus)(void);
	static FP_GetFocus &GetFocus;

	typedef HDC ( __stdcall * FP_GetDC)(HWND hWnd);
	static FP_GetDC &GetDC;

	typedef SHORT ( __stdcall * FP_GetKeyState)(int nVirtKey);
	static FP_GetKeyState &GetKeyState;

	typedef HWND ( * FP_GetActiveWindow)(void);
	static FP_GetActiveWindow &GetActiveWindow;

	typedef HWND ( * FP_GetCapture)(void);
	static FP_GetCapture &GetCapture;

	typedef int ( __stdcall * FP_GetDlgCtrlID)(HWND hWnd);
	static FP_GetDlgCtrlID &GetDlgCtrlID;

	typedef HWND ( __stdcall * FP_ChildWindowFromPointEx)(HWND, POINT, UINT);
	static FP_ChildWindowFromPointEx &ChildWindowFromPointEx;

	typedef BOOL ( __stdcall * FP_GetWindowRect)(HWND hWnd, LPRECT lpRect);
	static FP_GetWindowRect &GetWindowRect;

	typedef BOOL ( __stdcall * FP_GetCursorPos)(LPPOINT lpPoint);
	static FP_GetCursorPos &GetCursorPos;

	typedef BOOL ( __stdcall * FP_CloseWindow)(HWND hWnd);
	static FP_CloseWindow &CloseWindow;

	typedef BOOL ( __stdcall * FP_EndDialog)(HWND hDlg, int nResult);
	static FP_EndDialog &EndDialog;

	typedef HWND ( __stdcall * FP_SetFocus)(HWND hWnd);
	static FP_SetFocus &SetFocus;

	typedef BOOL ( __stdcall * FP_SetDlgItemTextA)(HWND hDlg, int nIDDlgItem, LPCSTR lpString);
	static FP_SetDlgItemTextA &SetDlgItemTextA;

	typedef int ( __stdcall * FP_DialogBoxParamA)(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
	static FP_DialogBoxParamA &DialogBoxParamA;

#ifdef _MSVC
	typedef int ( __stdcall * FP_DialogBoxIndirectParamA)(HINSTANCE hInstance, LPCDLGTEMPLATEA hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
	static FP_DialogBoxIndirectParamA &DialogBoxIndirectParamA;
#endif

	typedef int ( __stdcall * FP_ShowCursor)(BOOL bShow);
	static FP_ShowCursor &ShowCursor;

	typedef SHORT ( __stdcall * FP_GetAsyncKeyState)(int vKey);
	static FP_GetAsyncKeyState &GetAsyncKeyState;

	typedef int ( __stdcall * FP_ToAscii)(UINT uVirtKey, UINT uScanCode, PBYTE lpKeyState, LPWORD lpChar, UINT uFlags);
	static FP_ToAscii &ToAscii;

	typedef UINT ( __stdcall * FP_MapVirtualKeyA)(UINT uCode, UINT uMapType);
	static FP_MapVirtualKeyA &MapVirtualKeyA;

	typedef int ( __stdcall * FP_GetSystemMetrics)(int nIndex);
	static FP_GetSystemMetrics &GetSystemMetrics;

	typedef BOOL ( __stdcall * FP_SetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
	static FP_SetWindowPos &SetWindowPos;

	typedef BOOL ( __stdcall * FP_DestroyWindow)(HWND hWnd);
	static FP_DestroyWindow &DestroyWindow;

	typedef BOOL ( * FP_ReleaseCapture)(void);
	static FP_ReleaseCapture &ReleaseCapture;

	typedef HWND ( __stdcall * FP_SetCapture)(HWND hWnd);
	static FP_SetCapture &SetCapture;

	typedef BOOL ( __stdcall * FP_AdjustWindowRectEx)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
	static FP_AdjustWindowRectEx &AdjustWindowRectEx;

	typedef HMENU ( __stdcall * FP_GetMenu)(HWND hWnd);
	static FP_GetMenu &GetMenu;

	typedef BOOL ( __stdcall * FP_AdjustWindowRect)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
	static FP_AdjustWindowRect &AdjustWindowRect;

	typedef DWORD ( __stdcall * FP_GetSysColor)(int nIndex);
	static FP_GetSysColor &GetSysColor;

	typedef UINT ( __stdcall * FP_IsDlgButtonChecked)(HWND hDlg, int nIDButton);
	static FP_IsDlgButtonChecked &IsDlgButtonChecked;

	typedef BOOL ( __stdcall * FP_CheckDlgButton)(HWND hDlg, int nIDButton, UINT uCheck);
	static FP_CheckDlgButton &CheckDlgButton;

	typedef DWORD ( __stdcall * FP_WaitForInputIdle)(HANDLE hProcess, DWORD dwMilliseconds);
	static FP_WaitForInputIdle &WaitForInputIdle;

	typedef HWND ( __stdcall * FP_GetTopWindow)(HWND hWnd);
	static FP_GetTopWindow &GetTopWindow;

	typedef HWND ( * FP_GetForegroundWindow)(void);
	static FP_GetForegroundWindow &GetForegroundWindow;

	typedef HICON ( __stdcall * FP_LoadIconA)(HINSTANCE hInstance, LPCSTR lpIconName);
	static FP_LoadIconA &LoadIconA;

	typedef HWND ( __stdcall * FP_SetActiveWindow)(HWND hWnd);
	static FP_SetActiveWindow &SetActiveWindow;

	typedef BOOL ( __stdcall * FP_RedrawWindow)(HWND hWnd, const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags);
	static FP_RedrawWindow &RedrawWindow;

	typedef DWORD ( __stdcall * FP_GetWindowContextHelpId)(HWND);
	static FP_GetWindowContextHelpId &GetWindowContextHelpId;

	typedef BOOL ( __stdcall * FP_WinHelpA)(HWND hWndMain, LPCSTR lpszHelp, UINT uCommand, DWORD dwData);
	static FP_WinHelpA &WinHelpA;

	typedef HWND ( __stdcall * FP_ChildWindowFromPoint)(HWND hWndParent, POINT Point);
	static FP_ChildWindowFromPoint &ChildWindowFromPoint;

	typedef HCURSOR ( __stdcall * FP_LoadCursorA)(HINSTANCE hInstance, LPCSTR lpCursorName);
	static FP_LoadCursorA &LoadCursorA;

	typedef HCURSOR ( __stdcall * FP_SetCursor)(HCURSOR hCursor);
	static FP_SetCursor &SetCursor;

	typedef void ( __stdcall * FP_PostQuitMessage)(int nExitCode);
	static FP_PostQuitMessage &PostQuitMessage;

	typedef HWND ( __stdcall * FP_FindWindowA)(LPCSTR lpClassName, LPCSTR lpWindowName);
	static FP_FindWindowA &FindWindowA;

	typedef BOOL ( __stdcall * FP_SetCursorPos)(int X, int Y);
	static FP_SetCursorPos &SetCursorPos;

#ifdef _MSVC
	typedef HWND ( __stdcall * FP_CreateDialogIndirectParamA)(HINSTANCE hInstance, LPCDLGTEMPLATEA lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
	static FP_CreateDialogIndirectParamA &CreateDialogIndirectParamA;
#endif

	typedef int ( __stdcall * FP_GetKeyNameTextA)(LONG lParam, LPSTR lpString, int nSize);
	static FP_GetKeyNameTextA &GetKeyNameTextA;

	typedef BOOL ( __stdcall * FP_ScreenToClient)(HWND hWnd, LPPOINT lpPoint);
	static FP_ScreenToClient &ScreenToClient;

	typedef BOOL ( __stdcall * FP_LockWindowUpdate)(HWND hWndLock);
	static FP_LockWindowUpdate &LockWindowUpdate;

	typedef int ( __stdcall * FP_MessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
	static FP_MessageBoxA &MessageBoxA;

	typedef int ( __stdcall * FP_ReleaseDC)(HWND hWnd, HDC hDC);
	static FP_ReleaseDC &ReleaseDC;

	typedef HWND ( __stdcall * FP_WindowFromPoint)(POINT Point);
	static FP_WindowFromPoint &WindowFromPoint;

	typedef BOOL ( __stdcall * FP_UpdateWindow)(HWND hWnd);
	static FP_UpdateWindow &UpdateWindow;

	typedef LONG ( __stdcall * FP_SetWindowLongA)(HWND hWnd, int nIndex, LONG dwNewLong);
	static FP_SetWindowLongA &SetWindowLongA;

	typedef LONG ( __stdcall * FP_GetWindowLongA)(HWND hWnd, int nIndex);
	static FP_GetWindowLongA &GetWindowLongA;

	typedef BOOL ( __stdcall * FP_ValidateRect)(HWND hWnd, const RECT *lpRect);
	static FP_ValidateRect &ValidateRect;

	typedef BOOL ( __stdcall * FP_IntersectRect)(LPRECT lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2);
	static FP_IntersectRect &IntersectRect;

	typedef int ( __stdcall * FP_MessageBoxIndirectA)(LPMSGBOXPARAMSA);
	static FP_MessageBoxIndirectA &MessageBoxIndirectA;

	typedef BOOL ( __stdcall * FP_PeekMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
	static FP_PeekMessageA &PeekMessageA;

	typedef LRESULT ( __stdcall * FP_CallWindowProcA)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static FP_CallWindowProcA &CallWindowProcA;

	typedef BOOL ( __stdcall * FP_KillTimer)(HWND hWnd, UINT uIDEvent);
	static FP_KillTimer &KillTimer;

	typedef LONG ( __stdcall * FP_SendDlgItemMessageA)(HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam);
	static FP_SendDlgItemMessageA &SendDlgItemMessageA;

	typedef UINT ( __stdcall * FP_SetTimer)(HWND hWnd, UINT nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
	static FP_SetTimer &SetTimer;

	typedef BOOL ( __stdcall * FP_ShowWindow)(HWND hWnd, int nCmdShow);
	static FP_ShowWindow &ShowWindow;

	typedef BOOL ( __stdcall * FP_InvalidateRect)(HWND hWnd, const RECT *lpRect, BOOL bErase);
	static FP_InvalidateRect &InvalidateRect;

	typedef BOOL ( __stdcall * FP_EnableWindow)(HWND hWnd, BOOL bEnable);
	static FP_EnableWindow &EnableWindow;

	typedef LRESULT ( __stdcall * FP_SendMessageA)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static FP_SendMessageA &SendMessageA;

	typedef HWND ( __stdcall * FP_GetDlgItem)(HWND hDlg, int nIDDlgItem);
	static FP_GetDlgItem &GetDlgItem;

	typedef BOOL ( __stdcall * FP_PostMessageA)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static FP_PostMessageA &PostMessageA;

	typedef int ( * FP_wsprintfA)(LPSTR, LPCSTR, ...);
	static FP_wsprintfA &wsprintfA;

	typedef BOOL ( __stdcall * FP_SetRect)(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom);
	static FP_SetRect &SetRect;

	typedef BOOL ( __stdcall * FP_ClientToScreen)(HWND hWnd, LPPOINT lpPoint);
	static FP_ClientToScreen &ClientToScreen;

	typedef BOOL ( __stdcall * FP_TranslateMessage)(const MSG *lpMsg);
	static FP_TranslateMessage &TranslateMessage;

	typedef LONG ( __stdcall * FP_DispatchMessageA)(const MSG *lpMsg);
	static FP_DispatchMessageA &DispatchMessageA;

	typedef BOOL ( __stdcall * FP_GetClientRect)(HWND hWnd, LPRECT lpRect);
	static FP_GetClientRect &GetClientRect;

	typedef HWND ( __stdcall * FP_GetWindow)(HWND hWnd, UINT uCmd);
	static FP_GetWindow &GetWindow;

	typedef BOOL ( __stdcall * FP_BringWindowToTop)(HWND hWnd);
	static FP_BringWindowToTop &BringWindowToTop;

	typedef BOOL ( __stdcall * FP_SetForegroundWindow)(HWND hWnd);
	static FP_SetForegroundWindow &SetForegroundWindow;

	typedef HWND ( __stdcall * FP_CreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	static FP_CreateWindowExA &CreateWindowExA;

	typedef ATOM ( __stdcall * FP_RegisterClassA)(const WNDCLASSA *lpWndClass);
	static FP_RegisterClassA &RegisterClassA;

	typedef int ( __stdcall * FP_GetClassNameA)(HWND hWnd, LPSTR lpClassName, int nMaxCount);
	static FP_GetClassNameA &GetClassNameA;

	typedef BOOL ( __stdcall * FP_IsWindowVisible)(HWND hWnd);
	static FP_IsWindowVisible &IsWindowVisible;

	typedef BOOL ( __stdcall * FP_EnumChildWindows)(HWND hWndParent, WNDENUMPROC lpEnumFunc, LPARAM lParam);
	static FP_EnumChildWindows &EnumChildWindows;

	typedef BOOL ( __stdcall * FP_IsWindowEnabled)(HWND hWnd);
	static FP_IsWindowEnabled &IsWindowEnabled;

	typedef HWND ( __stdcall * FP_GetParent)(HWND hWnd);
	static FP_GetParent &GetParent;

	typedef HWND ( __stdcall * FP_GetNextDlgTabItem)(HWND hDlg, HWND hCtl, BOOL bPrevious);
	static FP_GetNextDlgTabItem &GetNextDlgTabItem;

	typedef BOOL ( __stdcall * FP_IsDialogMessageA)(HWND hDlg, LPMSG lpMsg);
	static FP_IsDialogMessageA &IsDialogMessageA;

	typedef int ( __stdcall * FP_TranslateAcceleratorA)(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg);
	static FP_TranslateAcceleratorA &TranslateAcceleratorA;

	typedef BOOL ( __stdcall * FP_CharToOemBuffA)(LPCSTR lpszSrc, LPSTR lpszDst, DWORD cchDstLength);
	static FP_CharToOemBuffA &CharToOemBuffA;

	typedef HDC ( __stdcall * FP_BeginPaint)(HWND hWnd, LPPAINTSTRUCT lpPaint);
	static FP_BeginPaint &BeginPaint;

	typedef BOOL ( __stdcall * FP_EndPaint)(HWND hWnd, const PAINTSTRUCT *lpPaint);
	static FP_EndPaint &EndPaint;

	typedef HWND ( __stdcall * FP_CreateDialogParamA)(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
	static FP_CreateDialogParamA &CreateDialogParamA;

	typedef int ( __stdcall * FP_GetWindowTextA)(HWND hWnd, LPSTR lpString, int nMaxCount);
	static FP_GetWindowTextA &GetWindowTextA;

	typedef BOOL ( __stdcall * FP_RegisterHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
	static FP_RegisterHotKey &RegisterHotKey;



};

class MovieInfo
{
public:
	// technically, this is a DVC<const char*>
	// and string management is done manually
	static constexpr reference<DynamicVectorClass<MovieInfo>, 0xABF390u> const Array{};

	bool operator== (MovieInfo const& rhs) const
	{
		return !_strcmpi(this->Name, rhs.Name);
	}

	explicit MovieInfo(const char* fname)
		: Name(fname ? _strdup(fname) : nullptr)
	{ }

	MovieInfo() : Name(nullptr)
	{ }

	~MovieInfo()
	{
		if(this->Name) {
			free(const_cast<char*>(this->Name));
		}
	}

	const char* Name; // yes, only that
};

struct MovieUnlockableInfo
{
	static constexpr reference<MovieUnlockableInfo, 0x832C20u, 1u> const Common{};
	static constexpr reference<MovieUnlockableInfo, 0x832C30u, 8u> const Allied{};
	static constexpr reference<MovieUnlockableInfo, 0x832CA0u, 8u> const Soviet{};

	MovieUnlockableInfo() = default;

	explicit MovieUnlockableInfo(const char* pFilename, const char* pDescription = nullptr, int disk = 2)
		: Filename(pFilename), Description(pDescription), DiskRequired(disk)
	{ }

	const char* Filename{ nullptr };
	const char* Description{ nullptr };
	int DiskRequired{ 2 };
};

namespace Unsorted
{
	// if != 0, EVA_SWxxxActivated is skipped
	static constexpr reference<int, 0xA8B538> MuteSWLaunches {};

	// skip unit selection and move command voices?
	static constexpr reference<bool, 0x822CF2> MoveFeedback {};

	static constexpr reference<byte, 0xA8ED6B> ArmageddonMode {};
	static constexpr reference<byte, 0xA8E9A0> WTFMode {};
	static constexpr constant_ptr<DynamicVectorClass<ObjectClass*>, 0x8A0360> ObjectsInLayers {};

// checkbox states, afaik
	static constexpr reference<byte, 0xA8B258> Bases {};
	static constexpr reference<byte, 0xA8B260> BridgeDestruction {};
	static constexpr reference<byte, 0xA8B261> Crates {};
	static constexpr reference<byte, 0xA8B262> ShortGame {};
	static constexpr reference<byte, 0xA8B263> SWAllowed {};
	static constexpr reference<byte, 0xA8B26C> MultiEngineer {};
	static constexpr reference<byte, 0xA8B31C> AlliesAllowed {};
	static constexpr reference<byte, 0xA8B31D> HarvesterTruce {};
	static constexpr reference<byte, 0xA8B31E> CTF {};
	static constexpr reference<byte, 0xA8B31F> FOW {};
	static constexpr reference<byte, 0xA8B320> MCVRedeploy {};

	static constexpr reference<TacticalSelectableStruct, 0xB0CEC8, 500> TacticalSelectables {};
	static constexpr reference<bool, 0xB0FE65> TypeSelecting {};

struct ColorPacker
{
	int _R_SHL;
	int _R_SHR;
	int _B_SHL;
	int _B_SHR;
	int _G_SHL;
	int _G_SHR;
};

static constexpr constant_ptr<ColorPacker, 0x8A0DD0> ColorPackData {};

static constexpr constant_ptr<CellStruct, 0xABD490> CellSpreadTable {};

static constexpr reference<int, 0x8809A0> CurrentSWType {};

static const int except_txt_length = 0xFFFF;
static constexpr constant_ptr<char, 0x8A3A08> except_txt_content {};

/*
 * This thing is ridiculous
 * all xxTypeClass::Create functions use it:

  // doing this makes no sense - it's just a wrapper around CTOR, which doesn't call any Mutex'd functions... but who cares
  InfantryTypeClass *foo = something;
  ++SomeMutex;
  InfantryClass *obj = foo->CreateObject();
  --SomeMutex;

  // XXX do not do this if you aren't sure if the object can exist in this place
  // - this flag overrides any placement checks so you can put Terror Drones into trees and stuff
  ++SomeMutex;
  obj->Unlimbo(blah);
  --SomeMutex;

  AI base node generation uses it:
  int level = SomeMutex;
  SomeMutex = 0;
  House->GenerateAIBuildList();
  SomeMutex = level;

  Building destruction uses it:
  if(!SomeMutex) {
  	Building->ShutdownSensorArray();
  	Building->ShutdownDisguiseSensor();
  }

  Building placement uses it:
  if(!SomeMutex) {
  	UnitTypeClass *freebie = Building->Type->FreeUnit;
  	if(freebie) {
  		freebie->CreateObject(blah);
  	}
  }

  Building state animations use it:
  if(SomeMutex) {
  	// foreach attached anim
  	// update anim state (normal | damaged | garrisoned) if necessary, play anim
  }

  building selling uses it:
  if(blah) {
  	++SomeMutex;
  	this->Type->UndeploysInto->CreateAtMapCoords(blah);
  	--SomeMutex;
  }

  Robot Control Centers use it:
  if ( !SomeMutex ) {
  	VoxClass::PlayFromName("EVA_RobotTanksOffline/BackOnline", -1, -1);
  }

  and so on...
 */
	// Note: SomeMutex has been renamed to this because it reflects the usage better
static constexpr reference<int, 0xA8E7AC> IKnowWhatImDoing {}; // h2ik
};

struct CheatData {
	bool * Destination;
	const char * TriggerString;
	DWORD unknown1;
	DWORD unknown2;
};

// this holds four original cheats, keep that limit in mind
static constexpr constant_ptr<CheatData, 0x825C28> OriginalCheats {};
