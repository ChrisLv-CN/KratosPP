#include "INIReader.h"

INIReader::INIReader(std::vector<std::string> dependency, const char *section)
{
	this->Dependency = dependency;
	this->Section = section;
}

INILinkedBuffer *INIReader::GetLinkedBuffer()
{
	if (!m_LinkedBuffer || m_LinkedBuffer->IsExpired())
	{
		m_LinkedBuffer = INIBufferManager::FindLinkedBuffer(Dependency, Section);
	}
	return m_LinkedBuffer;
}

