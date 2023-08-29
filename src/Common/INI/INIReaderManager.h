#pragma once

#include "INIConstant.h"
#include "INIBuffer.h"
#include "INIFileBuffer.h"
#include "INIConfig.h"
#include "INIReader.h"

#include <map>
#include <vector>

#include <SessionClass.h>

class INIReaderManager
{
public:
	struct LinkedBufferKey
	{
		std::vector<std::string> dependency;
		std::string section;

		auto operator <=>(const LinkedBufferKey&) const = default;
	};
	static void ClearBuffer(EventSystem* sender, Event e, void* args);

	/// <summary>
	/// 从INI文件中读取KV对，KV对以字符串形式缓存
	/// </summary>
	/// <param name="fileName"></param>
	/// <returns></returns>
	static INIFileBuffer* FindFile(std::string fileName);
	
	static INIBuffer* FindBuffer(std::string fileName, std::string section);

	/// <summary>
	/// 按顺序从ini文件组中，读取section的KV，连接成一个buffer。
	/// 从buffer中检索指定的key时，按顺序检索。
	/// </summary>
	/// <param name="dependency"></param>
	/// <param name="section"></param>
	/// <returns></returns>
	static INILinkedBuffer* FindLinkedBuffer(std::vector<std::string> dependency, std::string section);
private:


	// 每一个ini文件对应一个INIFileBuffer，为了保持顺序，用向量存储
	static std::vector<INIFileBuffer*> s_File;

	static std::map<LinkedBufferKey, INILinkedBuffer*> s_LinkedBuffer;
	// INILinkedBuffer里的未转换的kv对转换成对象后，用类名存储
	static std::map<INILinkedBuffer, std::map<std::string, INIConfigBase*>> s_Configs;
};

