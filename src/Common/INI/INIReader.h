#pragma once

#include "INIBuffer.h"
#include "INIReaderManager.h"

#include <CCINIClass.h>

/// <summary>
/// 读取指定dependency下的指定section的KV对
/// </summary>
class INIBufferReader
{
public:
	INIBufferReader(std::vector<std::string> dependency, const char* section);

	bool HasSection(const char* section);
private:
	INILinkedBuffer* GetBuffer();

	std::vector<std::string> m_Dependency;
	std::string m_Section;
	INILinkedBuffer* m_LinkedBuffer;
};
