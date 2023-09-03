﻿#pragma once

#include <typeinfo>
#include <string>
#include <format>
#include <stack>

#include <Common/Components/GameObject.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/Debug.h>
#include <Utilities/TemplateDef.h>

template <typename TBase, typename TExt>
class GOExtension
{
public:
	using base_type = TBase;

	class ExtData : public Extension<TBase>
	{
	public:
		ExtData(TBase *OwnerObject) : Extension<TBase>(OwnerObject)
		{
			m_GameObject = new GameObject(goName);
			m_GameObject->_OnAwake += newDelegate(this, &ExtData::Awake);
			// 读取全局脚本附加给GameObject，但不激活，等待GameObject首次访问触发Awake
			// Search and instantiate global script objects in TechnoExt
			TExt::AddGlobalScripts(&m_GlobalScripts, this);
			CreateGlobalScriptable();
		};

		~ExtData() override
		{
			m_GameObject->EnsureDestroy();
			delete m_GameObject;
		};

		virtual void InvalidatePointer(void *ptr, bool bRemoved) override
		{
			m_GameObject->Foreach([&ptr](Component *c)
								  { c->InvalidatePointer(ptr); });
		};

		virtual void LoadFromStream(ExStreamReader &stream) override
		{
			Extension<TBase>::LoadFromStream(stream);
			this->Serialize(stream);
			m_GameObject->Foreach([&stream](Component *c)
								  { c->LoadFromStream(stream); });
		};

		virtual void SaveToStream(ExStreamWriter &stream) override
		{
			Extension<TBase>::SaveToStream(stream);
			this->Serialize(stream);
			m_GameObject->Foreach([&stream](Component *c)
								  { c->SaveToStream(stream); });
		};

		//----------------------
		// GameObject
		GameObject *GetGameObject()
		{
			return m_GameObject->GetAwaked();
		};
		__declspec(property(get = GetGameObject)) GameObject *_GameObject;

	private:
		template <typename T>
		void Serialize(T &Stm){};

		//----------------------
		// GameObject
		std::string goName = typeid(TExt).name();
		GameObject *m_GameObject;

		//----------------------
		// Scripts

		/// <summary>
		///  call by GameObject _OnAwake Event
		/// </summary>
		void Awake(){};

		void CreateGlobalScriptable()
		{
			if (m_GlobalScriptsCreated)
			{
				return;
			}
			auto buffer = TakeBuffer();
			buffer.merge(m_GlobalScripts);
			for (Component *s : buffer)
			{
				TExt::ExtData::m_GameObject->AddComponentNotAwake(s);
			}
			GiveBackBuffer(buffer);
			m_GlobalScriptsCreated = true;
		}

		void CreateScriptable(std::list<Component *> *scripts)
		{
			if (m_ScriptsCreated)
			{
				return;
			}
			auto buffer = TakeBuffer();
			buffer.merge(*scripts);
			for (Component *s : buffer)
			{
				TExt::ExtData::m_GameObject->AddComponentNotAwake(s);
			}
			for (Component *s : buffer)
			{
				s->EnsureAwaked();
			}
			GiveBackBuffer(buffer);
			m_ScriptsCreated = true;
		}

		// 动态脚本
		bool m_ScriptsCreated = false;

		// 全局脚本
		bool m_GlobalScriptsCreated = false;
		std::list<Component *> m_GlobalScripts{};
	};

	class ExtContainer : public Container<TExt>
	{
	public:
		ExtContainer() : Container<TExt>(typeid(TExt).name()) {}
		~ExtContainer() = default;
	};

	//----------------------
	// Scripts Helper
	static std::list<Component *> TakeBuffer()
	{
		if (m_ScriptBuffer.empty())
		{
			m_ScriptBuffer.push(std::list<Component *>());
		}
		std::list<Component *> res = m_ScriptBuffer.top();
		m_ScriptBuffer.pop();
		return res;
	};

	static void GiveBackBuffer(std::list<Component *> &buffer)
	{
		buffer.clear();
		m_ScriptBuffer.push(buffer);
	};

	inline static std::stack<std::list<Component *>> m_ScriptBuffer{};
};

//----------------------
// Helper
template <typename TExt, typename TStatus, typename TBase>
static bool TryGetStatus(TBase *p, TStatus *status)
{
	auto *ext = TExt::ExtMap.Find(p);
	if (ext)
	{
		status = ext->_GameObject->GetComponent<TStatus>();
		return status;
	}
	return false;
}
template <typename TExt, typename TStatus, typename TBase>
static TStatus *GetStatus(TBase *p)
{
	TStatus *status = nullptr;
	TryGetStatus<TExt>(p, status);
	return status;
}
