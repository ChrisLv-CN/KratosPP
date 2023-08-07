/*
	Drawing related static class.
	This provides access to the game's Surfaces, color value conversion
	and text aligning helpers.
*/

#pragma once

#include <ColorScheme.h>
#include <GeneralDefinitions.h>
#include <Helpers/CompileTime.h>
#include <Surface.h>

struct DirtyAreaStruct
{
	RectangleStruct Rect;
	bool alphabool10;

	bool operator==(const DirtyAreaStruct& another) const
	{
		return
			Rect.X == another.Rect.X &&
			Rect.Y == another.Rect.Y &&
			Rect.Width == another.Rect.Width &&
			Rect.Height == another.Rect.Height &&
			alphabool10 == another.alphabool10;
	};
};

class Drawing
{
public:
	constexpr static reference<DynamicVectorClass<DirtyAreaStruct>, 0xB0CE78> DirtyAreas {};
	static constexpr reference<ColorStruct, 0xB0FA1Cu> const TooltipColor{};

	//TextBox dimensions for tooltip-style boxes
	static RectangleStruct* __fastcall GetTextDimensions(
		RectangleStruct* pOutBuffer, wchar_t const* pText, Point2D location,
		WORD flags, int marginX = 0, int marginY = 0)
			{ JMP_STD(0x4A59E0); }

	static RectangleStruct __fastcall GetTextDimensions(
		wchar_t const* pText, Point2D location, WORD flags, int marginX = 0,
		int marginY = 0)
	{
		RectangleStruct buffer;
		GetTextDimensions(&buffer, pText, location, flags, marginX, marginY);
		return buffer;
	}

	// Rectangles
	static RectangleStruct* __fastcall Intersect(
		RectangleStruct* pOutBuffer, RectangleStruct const& rect1,
		RectangleStruct const& rect2, int* delta_left = nullptr,
		int* delta_top = nullptr)
			{ JMP_STD(0x421B60); }

	static RectangleStruct __fastcall Intersect(
		RectangleStruct const& rect1, RectangleStruct const& rect2,
		int* delta_left = nullptr, int* delta_top = nullptr)
	{
		RectangleStruct buffer;
		Intersect(&buffer, rect1, rect2, delta_left, delta_top);
		return buffer;
	}

	// Rect1 will be changed, notice that - secsome
	static RectangleStruct* __fastcall Union(
		RectangleStruct* pOutBuffer,
		RectangleStruct& rect1,
		RectangleStruct& rect2)
			{ JMP_STD(0x487F40); }

	// Rect1 will be changed, notice that - secsome
	static RectangleStruct __fastcall Union(
		RectangleStruct& rect1,
		RectangleStruct& rect2)
	{
		RectangleStruct buffer;
		Union(&buffer, rect1, rect2);
		return buffer;
	}

	static DWORD __fastcall RGB2DWORD(int red, int green, int blue)
	{ JMP_STD(0x4355D0); }

	static DWORD RGB2DWORD(const ColorStruct Color)
	{
		return RGB2DWORD(Color.R, Color.G, Color.B);
	}

	//Stuff

	/** Message is a vswprintf format specifier, ... is for any arguments needed */
	static Point2D * __cdecl PrintUnicode(Point2D *Position1, wchar_t *Message, Surface *a3, RectangleStruct *Rect, Point2D *Position2,
			ColorScheme *a6, int a7, int a8, ...)
		{ JMP_STD(0x4A61C0); };
};

//A few preset 16bit colors.
#define		COLOR_BLACK  0x0000
#define		COLOR_WHITE  0xFFFF

#define		COLOR_RED    0xF800
#define		COLOR_GREEN  0x07E0
#define		COLOR_BLUE   0x001F

#define		COLOR_PURPLE (COLOR_RED | COLOR_BLUE)

class ABufferClass {
public:
	static constexpr reference<ABufferClass*, 0x87E8A4u> const ABuffer{};

	ABufferClass(RectangleStruct rect)
		{ JMP_THIS(0x410CE0); }

	RectangleStruct Bounds;
	int field_10;
	BSurface* Surface;
	WORD* BufferStart;
	WORD* BufferEnd;
	int BufferSizeInBytes;
	int field_24;
	int Width;
	int Height;
};

class ZBufferClass {
public:
	static constexpr reference<ZBufferClass*, 0x887644u> const ZBuffer{};

	ZBufferClass(RectangleStruct rect)
		{ JMP_THIS(0x7BC970); }

	RectangleStruct Bounds;
	int field_10;
	BSurface* Surface;
	WORD* BufferStart;
	WORD* BufferEnd;
	int BufferSizeInBytes;
	int field_24;
	int Width;
	int Height;
};
