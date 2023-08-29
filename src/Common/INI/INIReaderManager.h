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
	/// ��INI�ļ��ж�ȡKV�ԣ�KV�����ַ�����ʽ����
	/// </summary>
	/// <param name="fileName"></param>
	/// <returns></returns>
	static INIFileBuffer* FindFile(std::string fileName);
	
	static INIBuffer* FindBuffer(std::string fileName, std::string section);

	/// <summary>
	/// ��˳���ini�ļ����У���ȡsection��KV�����ӳ�һ��buffer��
	/// ��buffer�м���ָ����keyʱ����˳�������
	/// </summary>
	/// <param name="dependency"></param>
	/// <param name="section"></param>
	/// <returns></returns>
	static INILinkedBuffer* FindLinkedBuffer(std::vector<std::string> dependency, std::string section);
private:


	// ÿһ��ini�ļ���Ӧһ��INIFileBuffer��Ϊ�˱���˳���������洢
	static std::vector<INIFileBuffer*> s_File;

	static std::map<LinkedBufferKey, INILinkedBuffer*> s_LinkedBuffer;
	// INILinkedBuffer���δת����kv��ת���ɶ�����������洢
	static std::map<INILinkedBuffer, std::map<std::string, INIConfigBase*>> s_Configs;
};

