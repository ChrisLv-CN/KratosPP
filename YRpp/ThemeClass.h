#pragma once

#include <YRPPCore.h>
#include <ArrayClasses.h>
#include <Helpers/String.h>
#include <Helpers/CompileTime.h>

class AudioStream;

struct ThemeControl
{
	FixedString<0x100> ID;
	FixedString<0x100> Sound;
	FixedWString<0x40> UIName;
	int Scenario;
	float Length;
	bool Normal;
	bool Repeat;
	bool Exists;
	int Side;
};

class NOVTABLE ThemeClass
{
public:

	static constexpr reference<ThemeClass, 0xA83D10> Instance {};

	const char* GetID(unsigned int index) const
		{ JMP_THIS(0x721270) }

	const char* GetName(unsigned int index) const
		{ JMP_THIS(0x720940) }

	const char* GetFilename(unsigned int index) const
		{ JMP_THIS(0x720E10) }

	const wchar_t* GetUIName(unsigned int index) const
		{ JMP_THIS(0x7209B0) }

	int GetLength(unsigned int index) const
		{ JMP_THIS(0x720E50) }

	bool IsAvailable(int index) const
		{ JMP_THIS(0x721140) }

	bool IsNormal(int index) const
		{ JMP_THIS(0x7211E0) }

	int FindIndex(const char* pID) const
		{ JMP_THIS(0x721210) }

	int GetRandomIndex(unsigned int lastTheme) const
		{ JMP_THIS(0x720A80) }

	void Queue(int index)
		{ JMP_THIS(0x720B20) }

	int Play(int index)
		{ JMP_THIS(0x720BB0) }

	void Stop(bool fade = false)
		{ JMP_THIS(0x720EA0) }

	void Suspend()
		{ JMP_THIS(0x720F70) }

	void AI()
		{ JMP_THIS(0x7209D0) }

	int CurrentTheme; // the playing theme's index
	int LastTheme; // the theme that cannot be selected randomly
	int QueuedTheme; // the next theme to be played
	int Volume;
	bool IsScoreRepeat;
	bool IsFading;
	bool IsScoreShuffle;
	DynamicVectorClass<ThemeClass*> Themes; // the list of all themes
	AudioStream* Stream;
};