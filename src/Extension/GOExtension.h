#pragma once

#include <typeinfo>
#include <stack>

#include <Common/Components/GameObject.h>
#include <Common/Components/ScriptComponent.h>
#include <Ext/LaserTrail.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>

class LaserTrail;

template <typename TBase, typename TExt>
class GOExtension
{
public:
	using base_type = TBase;

	class ExtData : public Extension<TBase>
	{
	public:
		ExtData(TBase* OwnerObject) : Extension<TBase>(OwnerObject)
			, _LaserTrail(OwnerObject)
		{
			m_GameObject = (goName);

			std::list<ScriptComponent*> ss{ &_LaserTrail };
			CreateScriptable(ss);
		}

		~ExtData() override
		{
			m_GameObject.Destroy();
		};

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { };

		virtual void LoadFromStream(PhobosStreamReader& Stm) override
		{
			Extension<TBase>::LoadFromStream(Stm);
			this->Serialize(Stm);
		};
		virtual void SaveToStream(PhobosStreamWriter& Stm) override
		{
			Extension<TBase>::SaveToStream(Stm);
			this->Serialize(Stm);
		};

		GameObject* GetGameObject()
		{
			return m_GameObject.GetAwaked();
		}
		__declspec(property(get = GetGameObject)) GameObject* _GameObject;
		LaserTrail _LaserTrail;
	private:

		template <typename T>
		void Serialize(T& Stm)
		{
			//_GameObject.Foreach([](Component* c) {c->Serialize<T>(Stm); });
		};

		//----------------------
		// GameObject
		std::string goName = typeid(TExt).name();
		GameObject m_GameObject;

		//----------------------
		// Scripts
		void CreateScriptable(std::list<ScriptComponent*> scripts)
		{
			if (m_ScriptsCreated)
			{
				return;
			}
			auto buffer = TakeBuffer();
			// scripts == null ? _globalScripts : _globalScripts.Concat(scripts);
			for (ScriptComponent* s : scripts)
			{
				buffer.push_back(s);
			}
			for (ScriptComponent* s : buffer)
			{
				m_GameObject.AddComponent(s);
			}
			for (ScriptComponent* s : buffer)
			{
				s->EnsureAwaked();
			}
			GiveBackBuffer(buffer);
			m_ScriptsCreated = true;
		}

		bool m_ScriptsCreated;

		static std::list<ScriptComponent*> TakeBuffer()
		{
			if (m_ScriptBuffer.empty())
			{
				m_ScriptBuffer.push(std::list<ScriptComponent*>());
			}
			auto res = m_ScriptBuffer.top();
			m_ScriptBuffer.pop();
			return res;
		}

		static void GiveBackBuffer(std::list<ScriptComponent*> buffer)
		{
			buffer.clear();
			m_ScriptBuffer.push(buffer);
		}

		inline static std::stack<std::list<ScriptComponent*>> m_ScriptBuffer{};
	};

	class ExtContainer : public Container<TExt>
	{
	public:
		ExtContainer() : Container<TExt>(typeid(TExt).name())
		{ }
		~ExtContainer() = default;
	};
};
