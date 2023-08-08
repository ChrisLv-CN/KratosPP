#pragma once
#include <Windows.h>

#include <Common/EventSystems/EventSystem.h>

class CCINIClass;
class AbstractClass;

constexpr auto NONE_STR = "<none>";
constexpr auto NONE_STR2 = "none";
constexpr auto TOOLTIPS_SECTION = "ToolTips";
constexpr auto SIDEBAR_SECTION = "Sidebar";

class Kratos
{
public:
	static void CmdLineParse(char**, int);

	static CCINIClass* OpenConfig(const char*);
	static void CloseConfig(CCINIClass*&);

	//variables
	static HANDLE hInstance;

	static const size_t readLength = 2048;
	static char readBuffer[readLength];
	static wchar_t wideBuffer[readLength];
	static const char readDelims[4];

	static const char* AppIconPath;
	static const wchar_t* MessageLabel;
	static const wchar_t* VersionDescription;

	static void Clear();
	static void PointerGotInvalid(AbstractClass* const pInvalid, bool const removed);
	static HRESULT SaveGameData(IStream* pStm);
	static void LoadGameData(IStream* pStm);

	static void SendActiveMessage(EventSystem* sender, Event e, void* args);
	static void DrawVersionText(EventSystem* sender, Event e, void* args);

	class UI
	{
	public:
		static bool DisableEmptySpawnPositions;
		static bool ExtendedToolTips;
		static int MaxToolTipWidth;
		static bool ShowHarvesterCounter;
		static double HarvesterCounter_ConditionYellow;
		static double HarvesterCounter_ConditionRed;
		static bool ShowProducingProgress;

		static const wchar_t* CostLabel;
		static const wchar_t* PowerLabel;
		static const wchar_t* TimeLabel;
		static const wchar_t* HarvesterLabel;
	};

	class Config
	{
	public:
		static bool ToolTipDescriptions;
		static bool PrioritySelectionFiltering;
		static bool DevelopmentCommands;
	};
};
