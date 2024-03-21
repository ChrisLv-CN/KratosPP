#include "PrintTextManager.h"

#include <StringTable.h>

Point2D PrintTextManager::_fontSize{ 0, 0 };

Point2D PrintTextManager::GetFontSize()
{
	if (_fontSize.IsEmpty())
	{
		const wchar_t* temp = L"0123456789+-*/%";
		RectangleStruct fontRect = Drawing::GetTextDimensions(temp, Point2D::Empty, 0, 0, 0);
		int x = fontRect.Width / 15;
		_fontSize.X = x % 2 == 0 ? x : x + 1;
		_fontSize.Y = fontRect.Height;
	}
	return _fontSize;
}

Point2D PrintTextManager::GetFontSize(std::wstring font)
{
	Point2D fontSize = GetFontSize();
	RectangleStruct fontRect = Drawing::GetTextDimensions(font.c_str(), Point2D::Empty, 0, 0, 0);
	int x = fontRect.Width;
	fontSize.X = x % 2 == 0 ? x : x + 1;
	fontSize.Y = fontRect.Height;
	return fontSize;
}

/**
 *@brief 在指定位置打印一个文字
 *
 * @param text 内容
 * @param houseColor 颜色
 * @param data 格式
 * @param pos 位置
 * @param pBound 范围
 * @param pSurface 渲染器
 * @param isBuilding 是否是建筑
 */
void PrintTextManager::Print(std::wstring text, ColorStruct houseColor, PrintTextData data, Point2D pos, RectangleStruct* pBound, DSurface* pSurface, bool isBuilding)
{
	bool noNumbers = data.NoNumbers || data.SHPDrawStyle != SHPDrawStyle::NUMBER;
	LongText longText = LongText::EMPTY;
	auto it = LongTextStrings.find(uppercase(text));
	if (it != LongTextStrings.end())
	{
		longText = it->second;
		noNumbers = true;
	}
	// 渲染
	if (data.UseSHP)
	{
		// 使用Shp显示数字
		int zeroFrameIndex = data.ZeroFrameIndex; // shp时的起始帧序号
		Point2D imageSize = data.ImageSize; // shp时的图案大小
		// 获取字体横向位移值，即图像宽度，同时计算阶梯高度偏移
		int x = imageSize.X % 2 == 0 ? imageSize.X : imageSize.X + 1;
		int y = isBuilding ? x / 2 : 0;

		if (noNumbers)
		{
			// 使用长字符不使用数字
			std::string file{};
			int idx = 0;
			if (data.SHPDrawStyle == SHPDrawStyle::TEXT)
			{
				file = data.SHPFileName;
				idx = data.ZeroFrameIndex;
			}
			else
			{
				switch (longText)
				{
				case LongText::HIT:
					file = data.HitSHP;
					idx = data.HitIndex;
					break;
				case LongText::MISS:
					file = data.MissSHP;
					idx = data.MissIndex;
					break;
				case LongText::CRIT:
					file = data.CritSHP;
					idx = data.CritIndex;
					break;
				case LongText::GLANCING:
					file = data.GlancingSHP;
					idx = data.GlancingIndex;
					break;
				case LongText::BLOCK:
					file = data.BlockSHP;
					idx = data.BlockIndex;
					break;
				default:
					return;
				}
			}
			if (IsNotNone(file))
			{
				if (SHPStruct* pCustomSHP = FileSystem::LoadSHPFile(file.c_str()))
				{
					// 显示对应的帧
					pSurface->DrawSHP(FileSystem::PALETTE_PAL.get(), pCustomSHP, idx, &pos, pBound);
				}

			}
		}
		else
		{
			// 拆成单个字符
			for (wchar_t& t : text)
			{
				int frameIndex = zeroFrameIndex;
				int frameOffset = 0;
				// 找到数字或者字符对应的图像帧
				switch (t)
				{
				case L'0':
					frameOffset = 0;
					break;
				case L'1':
					frameOffset = 1;
					break;
				case L'2':
					frameOffset = 2;
					break;
				case L'3':
					frameOffset = 3;
					break;
				case L'4':
					frameOffset = 4;
					break;
				case L'5':
					frameOffset = 5;
					break;
				case L'6':
					frameOffset = 6;
					break;
				case L'7':
					frameOffset = 7;
					break;
				case L'8':
					frameOffset = 8;
					break;
				case L'9':
					frameOffset = 9;
					break;
				case L'+':
					frameOffset = 10;
					break;
				case L'-':
					frameOffset = 11;
					break;
				case L'*':
					frameOffset = 12;
					break;
				case L'/':
				case L'|':
					frameOffset = 13;
					break;
				case L'%':
					frameOffset = 14;
					break;
				}
				// 找到对应的帧序号
				frameIndex += frameOffset;
				if (IsNotNone(data.SHPFileName))
				{
					if (SHPStruct* pCustomSHP = FileSystem::LoadSHPFile(data.SHPFileName.c_str()))
					{
						// 显示对应的帧
						pSurface->DrawSHP(FileSystem::PALETTE_PAL.get(), pCustomSHP, frameIndex, &pos, pBound);
					}
				}
				// 调整下一个字符锚点
				pos.X += x;
				pos.Y -= y;
			}
		}
	}
	else
	{
		if (noNumbers && longText == LongText::EMPTY)
		{
			return;
		}
		// 使用文字显示数字
		ColorStruct textColor = data.Color; // 文字时渲染颜色
		if (data.IsHouseColor && !houseColor == Colors::Empty)
		{
			textColor = houseColor;
		}
		// 拆成单个字符
		for (wchar_t& t : text)
		{
			std::wstring tt{ t }; // 补末尾\0
			// 画阴影
			if (!data.ShadowOffset.IsEmpty())
			{
				Point2D shadow = pos + data.ShadowOffset;
				pSurface->DrawText(tt.c_str(), pBound, &shadow, Drawing::RGB_To_Int(data.ShadowColor));
			}
			pSurface->DrawText(tt.c_str(), pBound, &pos, Drawing::RGB_To_Int(textColor));
			// 获取字体横向位移值，即图像宽度，同时计算阶梯高度偏移
			Point2D fontSize = GetFontSize(tt);
			int x = fontSize.X;
			int y = isBuilding ? fontSize.X / 2 : 0;
			pos.X += x;
			pos.Y -= y;
		}
	}
}

void PrintTextManager::Print(int number, ColorStruct houseColor, PrintTextData data, Point2D pos, RectangleStruct* pBound, DSurface* pSurface, bool isBuilding)
{
	if (data.UseSHP && data.SHPDrawStyle == SHPDrawStyle::PROGRESS)
	{
		// 进度条
		std::string file = data.SHPFileName;
		int frameIndex = data.ZeroFrameIndex + number / data.Wrap;
		if (data.MaxFrameIndex >= 0)
		{
			frameIndex = std::min(data.MaxFrameIndex, frameIndex);
		}
		if (IsNotNone(file))
		{
			if (SHPStruct* pCustomSHP = FileSystem::LoadSHPFile(data.SHPFileName.c_str()))
			{
				// 显示对应的帧
				pSurface->DrawSHP(FileSystem::PALETTE_PAL.get(), pCustomSHP, frameIndex, &pos, pBound);
			}
		}
	}
	else
	{
		Print(std::to_wstring(number), houseColor, data, pos, pBound, pSurface, isBuilding);
	}
}

void PrintTextManager::PrintText(std::string text, ColorStruct houseColor, Point2D pos, PrintTextData data)
{
	std::wstring wText = String2WString(text);
	DSurface* pSurface = DSurface::Temp;
	RectangleStruct bound = pSurface->GetRect();
	bound.Height -= 34;
	if (InRect(pos, bound))
	{
		Print(wText, houseColor, data, pos, &bound, pSurface);
	}
}

void PrintTextManager::PrintText(std::string text, ColorStruct houseColor, CoordStruct location, PrintTextData data)
{
	Point2D pos = ToClientPos(location);
	PrintText(text, houseColor, pos, data);
}

void PrintTextManager::PrintText(std::string text, ColorStruct color, Point2D pos)
{
	PrintTextData data;
	data.Color = color;
	PrintText(text, color, pos, data);
}

void PrintTextManager::PrintText(std::string text, ColorStruct color, CoordStruct location)
{
	Point2D pos = ToClientPos(location);
	PrintText(text, color, pos);
}

void PrintTextManager::PrintNumber(int number, ColorStruct houseColor, Point2D pos, PrintTextData data)
{
	DSurface* pSurface = DSurface::Temp;
	RectangleStruct bound = pSurface->GetRect();
	bound.Height -= 34;
	if (InRect(pos, bound))
	{
		Print(number, houseColor, data, pos, &bound, pSurface);
	}
}

#pragma region Rolling Text
std::vector<RollingText> PrintTextManager::_rollingTexts{};

void PrintTextManager::Clear(EventSystem* sender, Event e, void* args)
{
	_rollingTexts.clear();
}

/**
 *@brief 添加一条待打印的漂浮文字
 *
 * @param text 内容
 * @param location 起始位置
 * @param offset 位置偏移
 * @param rollSpeed 滚动速度
 * @param duration 持续时间
 * @param data 格式
 */
void PrintTextManager::AddRollingText(std::wstring text, CoordStruct location, Point2D offset, int rollSpeed, int duration, PrintTextData data)
{
	if (!text.empty())
	{
		RollingText rollingText{ text, location, offset, rollSpeed, duration, data };
		_rollingTexts.emplace_back(rollingText);
	}
}

void PrintTextManager::PrintRollingText(EventSystem* sender, Event e, void* args)
{
	if (args) // RenderLate
	{
		DSurface* pSurface = DSurface::Temp;
		RectangleStruct bound = pSurface->GetRect();
		bound.Height -= 34;
		// 打印滚动文字
		int size = _rollingTexts.size();
		for (int i = 0; i < size; i++)
		{
			auto it = _rollingTexts.begin();
			RollingText rollingText = *it;
			_rollingTexts.erase(it);
			// 检查存活以及是否在视野内且没有被黑幕遮挡，然后渲染
			Point2D pos;
			if (rollingText.CanPrintAndGetPos(bound, pos))
			{
				// 获得锚点位置
				Point2D pos2 = pos + rollingText.Offset;
				Print(rollingText.Text, Colors::Empty, rollingText.Data, pos2, &bound, pSurface, false);
				_rollingTexts.emplace_back(rollingText);
			}
		}
	}
}
#pragma endregion

