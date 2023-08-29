#pragma once

#include <string>
#include <vector>
#include <map>

#include "INIBuffer.h"

/// <summary>
/// 从指定的ini文件读取所有的KV对，用string储存value
/// </summary>
class INIFileBuffer
{
public:
	INIFileBuffer();
	INIFileBuffer(std::string fileName);
	bool HasSection(std::string section);
	INIBuffer* GetSection(std::string section);

	// 游戏开局清空所有的缓存，重新读取
	void ClearBuffer();

	std::string FileName{};
private:
	// 字符串格式的一行KV对
	struct Buffer
	{
		std::string Key;
		std::string Value;
	};

	void InitCache();

	/// <summary>
	/// 当需要获取某个section的时候，再读取Cache中的内容存入。
	/// Key是Section
	/// </summary>
	std::map<std::string, INIBuffer*> m_Section{};
	/// <summary>
	/// 从文件中读入ini文件，即刻读取section的所有KV对进行缓存
	/// Key是Section
	/// </summary>
	std::map<std::string, std::vector<Buffer>*> m_SectionCache{};

};

