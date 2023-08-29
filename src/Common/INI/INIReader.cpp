#include "INIReader.h"

INIBufferReader::INIBufferReader(std::vector<std::string> dependency, const char* section)
{
	this->m_LinkedBuffer = INIReaderManager::FindLinkedBuffer(dependency, section);
}

bool INIBufferReader::HasSection(const char* section)
{
	if (GetBuffer())
	{
		for (std::string fileName : GetBuffer()->GetDependency())
		{
			if (INIReaderManager::FindFile(fileName)->HasSection(section))
			{
				return true;
			}
		}
	}
	return false;
}

INILinkedBuffer* INIBufferReader::GetBuffer()
{
	if (m_LinkedBuffer->IsExpired())
	{
		std::vector<std::string> dependency = m_LinkedBuffer->GetDependency();
		std::string section = m_LinkedBuffer->GetSection();
		m_LinkedBuffer = INIReaderManager::FindLinkedBuffer(dependency, section);
	}
	return m_LinkedBuffer;
}
