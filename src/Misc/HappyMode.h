#pragma once
#pragma comment( lib, "cryptlib.lib" )

#include <iostream>
#include <aes.h>

#include <Common.h>
#include <CCINIClass.h>

using namespace CryptoPP;

class HappyMode
{
public:
	static bool Check();
};

