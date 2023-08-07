#pragma once

#include <GeneralStructures.h>
#include <YRDDraw.h>
#include <YRAllocator.h>

#include <Helpers/CompileTime.h>

class ConvertClass;
struct SHPStruct;

class NOVTABLE Surface
{
public:
	virtual ~Surface() { PUSH_IMM(SDDTOR_NODELETE); THISCALL(0x4115D0); }

	//Surface
	virtual bool CopyFromWhole(Surface* pSrc, bool bUnk1, bool bUnk2) R0;

	virtual bool CopyFromPart(
		RectangleStruct* pClipRect, //ignored and retrieved again...
		Surface* pSrc,
		RectangleStruct* pSrcRect,	//desired source rect of pSrc ?
		bool bUnk1,
		bool bUnk2) R0;

	virtual bool CopyFrom(
		RectangleStruct* pClipRect,
		RectangleStruct* pClipRect2,	//again? hmm
		Surface* pSrc,
		RectangleStruct* pDestRect,	//desired dest rect of pSrc ? (stretched? clipped?)
		RectangleStruct* pSrcRect,	//desired source rect of pSrc ?
		bool bUnk1,
		bool bUnk2) R0;

	virtual bool FillRectEx(RectangleStruct* pClipRect, RectangleStruct* pFillRect, COLORREF nColor) R0;

	virtual bool FillRect(RectangleStruct* pFillRect, COLORREF nColor) R0;

	virtual bool Fill(COLORREF nColor) R0;

	virtual bool FillRectTrans(RectangleStruct* pClipRect, ColorStruct Color, COLORREF nUnknown) R0;

	virtual bool DrawEllipse(
		int XOff, int YOff, int CenterX, int CenterY, RectangleStruct Rect, COLORREF nColor) R0;

	virtual bool SetPixel(Point2D* pPoint, COLORREF nColor) R0;

	virtual COLORREF GetPixel(Point2D* pPoint) R0;

	virtual bool DrawLineEx(RectangleStruct* pClipRect, Point2D* pStart, Point2D* pEnd, COLORREF nColor) R0;

	virtual bool DrawLine(Point2D* pStart, Point2D* pEnd, COLORREF nColor) R0;

	virtual bool DrawLineColor_AZ(
		RectangleStruct* pRect, Point2D* pStart, Point2D* pEnd, COLORREF nColor,
		DWORD dwUnk1, DWORD dwUnk2, bool bUnk) R0;

	virtual bool DrawMultiplyingLine_AZ(
		RectangleStruct* pRect, Point2D* pStart, Point2D* pEnd, DWORD dwMultiplier,
		DWORD dwUnk1, DWORD dwUnk2, bool bUnk) R0;

	virtual bool DrawSubtractiveLine_AZ(
		RectangleStruct* pRect, Point2D* pStart, Point2D* pEnd, ColorStruct* pColor,
		DWORD dwUnk1, DWORD dwUnk2, bool bUnk1, bool bUnk2,
		bool bUkn3, bool bUkn4, float fUkn) R0;

	virtual bool DrawRGBMultiplyingLine_AZ(
		RectangleStruct* pRect, Point2D* pStart, Point2D* pEnd, ColorStruct* pColor,
		float Intensity, DWORD dwUnk1, DWORD dwUnk2) R0;

	virtual bool PlotLine(
		RectangleStruct* pRect, Point2D* pStart, Point2D* pEnd, bool(__fastcall* fpDrawCallback)(int*)) R0;

	virtual bool DrawDashedLine(
		Point2D* pStart, Point2D* pEnd, int nColor, bool* Pattern, int nOffset) R0;

	virtual bool DrawDashedLine_(
		Point2D* pStart, Point2D* pEnd, int nColor, bool* Pattern, int nOffset, bool bUkn) R0;

	virtual bool DrawLine_(Point2D* pStart, Point2D* pEnd, int nColor, bool bUnk) R0;

	virtual bool DrawRectEx(RectangleStruct* pClipRect, RectangleStruct* pDrawRect, int nColor) R0;

	virtual bool DrawRect(RectangleStruct* pDrawRect, DWORD dwColor) R0;

	virtual void* Lock(int X, int Y) R0;

	virtual bool Unlock() R0;

	virtual bool CanLock(DWORD dwUkn1 = 0, DWORD dwUkn2 = 0) R0;

	virtual bool vt_entry_68(DWORD dwUnk1, DWORD dwUnk2) R0; // {JMP_THIS(0x411500);}

	virtual bool IsLocked() R0;

	virtual int GetBytesPerPixel() R0;

	virtual int GetPitch() R0;	//Bytes per scanline

	virtual RectangleStruct* GetRect(RectangleStruct* pRect) R0;

	virtual int GetWidth() R0;

	virtual int GetHeight() R0;

	virtual bool IsDSurface() R0; // guessed - secsome

	// Helper
	RectangleStruct GetRect()
	{
		RectangleStruct ret;
		this->GetRect(&ret);
		return ret;
	}


	// Properties

	int Width;
	int Height;
};

class NOVTABLE XSurface : public Surface
{
public:
	virtual bool PutPixelClip(Point2D* pPoint, short nUkn, RectangleStruct* pRect) R0;

	virtual short GetPixelClip(Point2D* pPoint, RectangleStruct* pRect) R0;

	int LockLevel;
	int BytesPerPixel;
};

class NOVTABLE BSurface : public XSurface
{
public:
	static constexpr constant_ptr<BSurface, 0xB2D928> VoxelSurface {};

	MemoryBuffer Buffer;
};

enum class ZGradientDescIndex : unsigned int
{
	Flat = 0,
	Not_Common = 1,
	Vertical = 2,
	Not_Common_2 = 3
};

// Comments from thomassneddon
static void __fastcall CC_Draw_Shape(Surface* Surface, ConvertClass* Palette, SHPStruct* SHP, int FrameIndex,
	const Point2D* const Position, const RectangleStruct* const Bounds, BlitterFlags Flags,
	int Remap,
	int ZAdjust, // + 1 = sqrt(3.0) pixels away from screen
	ZGradientDescIndex ZGradientDescIndex,
	int Brightness, // 0~2000. Final color = saturate(OriginalColor * Brightness / 1000.0f)
	int TintColor, SHPStruct* ZShape, int ZShapeFrame, int XOffset, int YOffset)
{
	JMP_STD(0x4AED70);
}

static Point2D* Fancy_Text_Print_Wide(Point2D* RetVal, const wchar_t* Text, Surface* Surface, RectangleStruct* Bounds,
	Point2D* Location, COLORREF ForeColor, COLORREF BackColor, TextPrintType Flag, ...)
{
	JMP_STD(0x4A60E0);
}

//static Point2D* __fastcall Simple_Text_Print_Wide(Point2D* RetVal, const wchar_t* Text, Surface* Surface, RectangleStruct* Bounds,
//	Point2D* Location, COLORREF ForeColor, COLORREF BackColor, TextPrintType Flag, bool bUkn)
//{
//	JMP_STD(0x4A5EB0);
//}

class NOVTABLE DSurface : public XSurface
{
public:
	static constexpr reference<DSurface*, 0x8872FCu> const Tile{};
	static constexpr reference<DSurface*, 0x887300u> const Sidebar{};
	static constexpr reference<DSurface*, 0x887308u> const Primary{};
	static constexpr reference<DSurface*, 0x88730Cu> const Hidden{};
	static constexpr reference<DSurface*, 0x887310u> const Alternate{};
	static constexpr reference<DSurface*, 0x887314u> const Temp{};
	static constexpr reference<DSurface*, 0x88731Cu> const Composite{};

	static constexpr reference<RectangleStruct, 0x886F90u> const SidebarBounds{};
	static constexpr reference<RectangleStruct, 0x886FA0u> const ViewBounds{};
	static constexpr reference<RectangleStruct, 0x886FB0u> const WindowBounds{};

	virtual bool DrawGradientLine(RectangleStruct* pRect, Point2D* pStart, Point2D* pEnd,
		ColorStruct* pStartColor, ColorStruct* pEndColor, float fStep, int nColor) R0;

	virtual bool CanBlit() R0;

	// Comments from thomassneddon
	void DrawSHP(ConvertClass* Palette, SHPStruct* SHP, int FrameIndex,
		const Point2D* const Position, const RectangleStruct* const Bounds, BlitterFlags Flags, int Remap,
		int ZAdjust, // + 1 = sqrt(3.0) pixels away from screen
		ZGradientDescIndex ZGradientDescIndex,
		int Brightness, // 0~2000. Final color = saturate(OriginalColor * Brightness / 1000.0f)
		int TintColor, SHPStruct* ZShape, int ZShapeFrame, int XOffset, int YOffset)
	{
		CC_Draw_Shape(this, Palette, SHP, FrameIndex, Position, Bounds, Flags, Remap, ZAdjust,
			ZGradientDescIndex, Brightness, TintColor, ZShape, ZShapeFrame, XOffset, YOffset);
	}

	void DrawText(const wchar_t* pText, RectangleStruct* pBounds, Point2D* pLocation,
		COLORREF ForeColor, COLORREF BackColor, TextPrintType Flag)
	{
		Point2D tmp = { 0, 0 };

		Fancy_Text_Print_Wide(&tmp, pText, this, pBounds, pLocation, ForeColor, BackColor, Flag);
	}

	void DrawText(const wchar_t* pText, Point2D* pLoction, COLORREF Color)
	{
		RectangleStruct rect = { 0, 0, 0, 0 };
		this->GetRect(&rect);

		Point2D tmp { 0,0 };
		Fancy_Text_Print_Wide(&tmp, pText, this, &rect, pLoction, Color, 0, TextPrintType::NoShadow);
	}

	void DrawText(const wchar_t* pText, int X, int Y, COLORREF Color)
	{
		Point2D P = { X ,Y };
		DrawText(pText, &P, Color);
	}

	void* Buffer;
	bool IsAllocated;
	bool IsInVideoRam;
	PROTECTED_PROPERTY(char, field_1A[2]);
	IDirectDrawSurface* VideoSurfacePtr;
	DDSURFACEDESC2* VideoSurfaceDescription;
};