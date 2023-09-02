#include "INIReader.h"

INIReader::INIReader(std::vector<std::string> dependency, const char *section)
{
	this->m_Dependency = dependency;
	this->m_Section = section;
}

INILinkedBuffer *INIReader::GetLinkedBuffer()
{
	if (!m_LinkedBuffer || m_LinkedBuffer->IsExpired())
	{
		m_LinkedBuffer = INIBufferManager::FindLinkedBuffer(m_Dependency, m_Section);
	}
	return m_LinkedBuffer;
}

bool INIReader::HasSection(const char *section)
{
	if (GetLinkedBuffer())
	{
		for (std::string fileName : GetLinkedBuffer()->GetDependency())
		{
			if (INIBufferManager::FindFile(fileName)->HasSection(section))
			{
				return true;
			}
		}
	}
	return false;
}
