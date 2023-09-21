#pragma once

#include <map>
#include <vector>

#include "INIConstant.h"
#include "INIBuffer.h"
#include "INIFileBuffer.h"

namespace INIBufferManager
{
	struct LinkedBufferKey
	{
		std::vector<std::string> dependency;
		std::string section;

		auto operator<=>(const LinkedBufferKey &) const = default;
	};

	// 每一个ini文件对应一个INIFileBuffer，为了保持顺序，用向量存储
	static std::vector<INIFileBuffer*> s_File{};

	static std::map<LinkedBufferKey, INILinkedBuffer*> s_LinkedBuffer{};

	static void ClearBuffer(EventSystem *sender, Event e, void *args)
	{
		// 释放INIFileBuffer
		for (auto fileBuffer : s_File)
		{
			fileBuffer->ClearBuffer();
		}
		s_File.clear();
		// 释放LinkedBuffer
		for (auto linkedBuffer : s_LinkedBuffer)
		{
			//linkedBuffer.second->Expired();
			GameDelete(linkedBuffer.second);
		}
		s_LinkedBuffer.clear();
	}

	/// <summary>
	/// 从INI文件中读取KV对，KV对以字符串形式缓存
	/// </summary>
	/// <param name="fileName"></param>
	/// <returns></returns>
	static INIFileBuffer *FindFile(std::string fileName)
	{
		for (auto buffer : s_File)
		{
			if (buffer->FileName == fileName)
			{
				return buffer;
			}
		}
		// 为每个iniFile创建一个存储对象
		// INIFileBuffer在构建时会读取KV对，以字符串形式缓存
		std::string f = fileName.data();
		INIFileBuffer* buffer = GameCreate<INIFileBuffer>(f);
		s_File.push_back(buffer);
		return buffer;
	}

	static INIBuffer *FindBuffer(std::string fileName, std::string section)
	{
		// ini文件按顺序储存，在查找时先读取Map.ini，GameMode.ini，最后读取Rules.ini
		return FindFile(fileName)->GetSection(section);
	}

	/// <summary>
	/// 按顺序从ini文件组中，读取section的KV，连接成一个buffer。
	/// 从buffer中检索指定的key时，按顺序检索。
	/// </summary>
	/// <param name="dependency"></param>
	/// <param name="section"></param>
	/// <returns></returns>
	static INILinkedBuffer *FindLinkedBuffer(std::vector<std::string> dependency, std::string section)
	{
		const LinkedBufferKey key{ dependency, section };
		auto it = s_LinkedBuffer.find(key);
		if (it != s_LinkedBuffer.end())
		{
			return it->second;
		}
		INILinkedBuffer* linkedBuffer{};
		// 缓存的是最后一个对象，所以是和Dependency的顺序相反，要倒着构建
		std::vector<std::string>::reverse_iterator rit;
		for (rit = dependency.rbegin(); rit != dependency.rend(); rit++)
		{
			std::string iniFileName = *rit;
			INIBuffer* buffer = FindBuffer(iniFileName, section);
			linkedBuffer = GameCreate<INILinkedBuffer>(buffer, linkedBuffer);
		}
		s_LinkedBuffer[key] = linkedBuffer;

		return linkedBuffer;
	}

};
