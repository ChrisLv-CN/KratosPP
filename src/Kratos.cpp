#include <Helpers/Macro.h>

#include <CCINIClass.h>
#include <Unsorted.h>
#include <Drawing.h>
#include <HouseClass.h>
#include <MessageListClass.h>

#include <exception>
#include <string>

#include <Kratos.h>
#include <KratosLib.h>

void Kratos::ExeRun(EventSystem* sender, Event e, void* args)
{
	KratosLib::Load();
}

void Kratos::SendActiveMessage(EventSystem* sender, Event e, void* args)
{
	if (args)
	{
#ifdef DEBUG
		const wchar_t* message = L"Debug Version " VERSION_SHORT_WSTR L" is active, have fun.";
		if (InChinese)
		{
			message = L"测试版本 " VERSION_SHORT_WSTR L" 已经激活，愉快的玩耍吧。";
		}
#else // DEBUG
		const wchar_t* message = L"Version " VERSION_SHORT_WSTR L" is active, have fun.";
		if (InChinese)
		{
			message = L"版本 " VERSION_SHORT_WSTR L" 已经激活，愉快的玩耍吧。";
		}
#endif // DEBUG
		MessageListClass::Instance->PrintMessage(L"[" PRODUCT_NAME L"]", message, 150, HouseClass::CurrentPlayer->ColorSchemeIndex, true);
		sender->RemoveHandler(e, SendActiveMessage);
	}
}

Kratos::VersionText Kratos::_versionText{};

void Kratos::DrawVersionText(EventSystem* sender, Event e, void* args)
{
	if (!_versionText.text)
	{
		_versionText.text = L"Kratos " VERSION_SHORT_WSTR;
		RectangleStruct textRect = Drawing::GetTextDimensions(_versionText.text, { 0, 0 }, 0, 2, 0);
		RectangleStruct sidebarRect = DSurface::Sidebar->GetRect();
		int x = sidebarRect.Width / 2 - textRect.Width / 2;
		int y = sidebarRect.Height - textRect.Height - textRect.Height / 4;
		_versionText.pos = { x, y };
		_versionText.color = Drawing::RGB_To_Int(Drawing::TooltipColor);
	}
	DSurface::Sidebar->DrawText(_versionText.text, &_versionText.pos, _versionText.color);
}
