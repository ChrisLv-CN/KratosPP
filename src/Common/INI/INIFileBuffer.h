#pragma once

#include <string>
#include <vector>
#include <map>

#include "INIBuffer.h"

/// <summary>
/// ��ָ����ini�ļ���ȡ���е�KV�ԣ���string����value
/// </summary>
class INIFileBuffer
{
public:
	INIFileBuffer();
	INIFileBuffer(std::string fileName);
	bool HasSection(std::string section);
	INIBuffer* GetSection(std::string section);

	// ��Ϸ����������еĻ��棬���¶�ȡ
	void ClearBuffer();

	std::string FileName{};
private:
	// �ַ�����ʽ��һ��KV��
	struct Buffer
	{
		std::string Key;
		std::string Value;
	};

	void InitCache();

	/// <summary>
	/// ����Ҫ��ȡĳ��section��ʱ���ٶ�ȡCache�е����ݴ��롣
	/// Key��Section
	/// </summary>
	std::map<std::string, INIBuffer*> m_Section{};
	/// <summary>
	/// ���ļ��ж���ini�ļ������̶�ȡsection������KV�Խ��л���
	/// Key��Section
	/// </summary>
	std::map<std::string, std::vector<Buffer>*> m_SectionCache{};

};

