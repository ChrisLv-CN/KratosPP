#include "INIBuffer.h"

INIBuffer::INIBuffer() {}

INIBuffer::INIBuffer(std::string fileName, std::string section)
{
	this->FileName = fileName;
	this->Section = section;
}

void INIBuffer::ClearBuffer()
{
	Unparsed.clear();
	Parsed.clear();
}

INILinkedBuffer::INILinkedBuffer()
{

}

INILinkedBuffer::INILinkedBuffer(INIBuffer* buffer, INILinkedBuffer* nextBuffer)
{
	m_Buffer = buffer;
	m_LinkedBuffer = nextBuffer;
}

std::vector<std::string> INILinkedBuffer::GetDependency()
{
	std::vector<std::string> dependency;
	GetDependency(dependency);
	return dependency;
}

void INILinkedBuffer::GetDependency(std::vector<std::string> &dependency)
{
	dependency.push_back(m_Buffer->FileName);
	if (m_LinkedBuffer)
	{
		m_LinkedBuffer->GetDependency(dependency);
	}
}

std::string INILinkedBuffer::GetSection()
{
	return m_Buffer->Section;
}

void INILinkedBuffer::ClearBuffer()
{

}

bool INILinkedBuffer::IsExpired()
{
	return this->_expired;
}

void INILinkedBuffer::Expired()
{
	this->_expired = true;
}



INIBuffer* INILinkedBuffer::GetFirstOccurrence(const char* key)
{
	auto it = m_Buffer->Unparsed.find(key);
	if (it != m_Buffer->Unparsed.end())
	{
		return m_Buffer;
	}
	if (m_LinkedBuffer)
	{
		return m_LinkedBuffer->GetFirstOccurrence(key);
	}
	return nullptr;
}

bool INILinkedBuffer::GetUnparsed(const char* key, std::string& val)
{
	if (m_Buffer->GetUnparsed(key, val))
	{
		return true;
	}
	if (m_LinkedBuffer)
	{
		return m_LinkedBuffer->GetUnparsed(key, val);
	}
	return false;
}
