#include <Helpers/Macro.h>

#include <CCINIClass.h>
#include <Unsorted.h>
#include <Drawing.h>
#include <HouseClass.h>
#include <MessageListClass.h>

#include <exception>
#include <string>

#include <Kratos.h>

Kratos::VersionText Kratos::_versionText{};
bool Kratos::_disableHappyMode = false;
CDTimerClass Kratos::_happyModeDelay{};
int Kratos::_happyModeMessageIndex = 7;

void Kratos::SendActiveMessage(EventSystem* sender, Event e, void* args)
{
	if (args)
	{
		const wchar_t* message = L"Version " VERSION_SHORT_WSTR L" is active, have fun.";
		if (InChinese)
		{
			message = L"版本 " VERSION_SHORT_WSTR L" 已经激活，愉快的玩耍吧。";
		}
		MessageListClass::Instance->PrintMessage(L"[" PRODUCT_NAME L"]", message, 150, HouseClass::CurrentPlayer->ColorSchemeIndex, true);
		sender->RemoveHandler(e, SendActiveMessage);
		if (Kratos::EnableHappyMode())
		{
			EventSystems::Render.AddHandler(Events::GScreenRenderEvent, Kratos::DrawNotAllowed);
			EventSystems::Logic.AddHandler(Events::LogicUpdateEvent, Kratos::HappyMode);
		}
	}
}

void Kratos::DrawVersionText(EventSystem* sender, Event e, void* args)
{
	if (!_versionText.text)
	{
		_versionText.text = L"KratosPP ver." VERSION_SHORT_WSTR;
		RectangleStruct textRect = Drawing::GetTextDimensions(_versionText.text, { 0, 0 }, 0, 2, 0);
		RectangleStruct sidebarRect = DSurface::Sidebar->GetRect();
		int x = sidebarRect.Width / 2 - textRect.Width / 2;
		int y = sidebarRect.Height - textRect.Height - textRect.Height / 2;
		_versionText.pos = { x, y };
		_versionText.color = Drawing::RGB_To_Int(Drawing::TooltipColor);
	}
	DSurface::Sidebar->DrawText(_versionText.text, &_versionText.pos, _versionText.color);
}

void Kratos::DrawNotAllowed(EventSystem* sender, Event e, void* args)
{
	if (args)
	{
		const wchar_t* text = L"yOu ArE nOt PeRmItTeD tO mAkE a MoD oN oThEr PeOpLe'S wOrK !!!11!!!";
		RectangleStruct textRect = Drawing::GetTextDimensions(text, {0,0}, 0, 2, 0);
		RectangleStruct rect = DSurface::Temp->GetRect();
		int x = rect.Width / 2 - textRect.Width / 2;
		int y = rect.Height / 2 - textRect.Height / 2;
		Point2D pos = { x, y };
		DSurface::Temp->DrawText(text, &pos, Drawing::RGB_To_Int(Drawing::TooltipColor));
	}
}

void Kratos::HappyMode(EventSystem* sender, Event e, void* args)
{
	if (args)
	{
		std::wstring message;
		switch (_happyModeMessageIndex)
		{
		case 7:
			message = L"The system has detected that you are not welcome.";
			if (InChinese)
			{
				message = L"系统检测到您是不受欢迎的对象。";
			}
			VoxClass::Play("EVA_NuclearSiloDetected");
			break;
		case 6:
			message = L"Self-Destruction countdown...";
			if (InChinese)
			{
				message = L"自毁倒计时开始...";
			}
			VoxClass::Play("Mis_A12_EvaCountdown");
			break;
		case 5:
		{
			message = std::to_wstring(_happyModeMessageIndex);
			int nukeSiren = VocClass::FindIndex("NukeSiren");
			if (nukeSiren > -1)
			{
				VocClass::PlayGlobal(nukeSiren, 0x2000, 1.0f);
			}
		}
			break;
		default:
			message = std::to_wstring(_happyModeMessageIndex);
			break;
		}
		if (_happyModeDelay.Expired())
		{
			_happyModeDelay.Start(90);
			if (_happyModeMessageIndex > 0)
			{
				MessageListClass::Instance->PrintMessage(L"[" PRODUCT_NAME L"]", message.c_str(), 450, HouseClass::CurrentPlayer->ColorSchemeIndex, true);
			}
			else if (_happyModeMessageIndex == 0)
			{
				message = L"Happy Mode Active !!!";
				if (InChinese)
				{
					message = L"愉悦模式已经激活 ！！！";
				}
				MessageListClass::Instance->PrintMessage(L"[" PRODUCT_NAME L"]", message.c_str(), -1, HouseClass::CurrentPlayer->ColorSchemeIndex, true);
			}
			_happyModeMessageIndex--;
		}
		if (_happyModeMessageIndex < 0)
		{

		}
	}
}

bool Kratos::EnableHappyMode()
{
	return !_disableHappyMode && HappyMode::Check();
}
