#include <Misc/HappyMode.h>

bool HappyMode::Check()
{
	Debug::Log("*********************************");
	Debug::Log(std::to_string(AES::BLOCKSIZE).c_str());
	Debug::Log("*********************************");
	MessageBoxW(NULL,
				std::to_wstring(AES::BLOCKSIZE).c_str(),
				L"调试信息XX", MB_OK);
	return true;
}
