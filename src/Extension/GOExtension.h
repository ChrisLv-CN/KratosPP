#pragma once

#include <typeinfo>
#include <string>
#include <format>
#include <stack>

#include <Common/Components/GameObject.h>

#include <Utilities/Container.h>
#include <Utilities/Debug.h>

class IExtData
{
public:
#ifdef DEBUG
	std::string thisId{};
	std::string thisName{};

	std::string baseId{};
	std::string baseName{};
#endif // DEBUG

	std::string ExtName{};
	std::string BaseName{};

	virtual std::string GetExtName() = 0;
	virtual std::string GetBaseName() = 0;

	virtual GameObject* GetGameObject() = 0;

};

template <typename TBase, typename TExt>
class GOExtension
{
public:
	using base_type = TBase;

	class ExtData : public Extension<TBase>, public IExtData
	{
	public:
		ExtData(TBase* OwnerObject) : Extension<TBase>(OwnerObject)
		{
			this->ExtName = typeid(TExt).name();
			this->BaseName = typeid(TBase).name();
#ifdef DEBUG
			char t_this[1024];
			sprintf_s(t_this, "%p", this);
			this->thisId = { t_this };

			this->thisName = typeid(TExt).name();

			char t_base[1024];
			sprintf_s(t_base, "%p", OwnerObject);
			this->baseId = { t_base };

			this->baseName = typeid(TBase).name();

			m_GameObject.extId = this->thisId;
			m_GameObject.extName = this->thisName;
			m_GameObject.baseId = this->baseId;
			m_GameObject.baseName = this->baseName;
#endif // DEBUG
			// 为了保证读存档的key一致，除GO外都不进行实例化
			// m_GameObject = new GameObject(goName);
			// m_GameObject = std::make_unique<GameObject>();
			m_GameObject.Name = typeid(TExt).name();
			m_GameObject.Name.append("_GameObject");
			// 附加Components但是不激活
			// 不从存档读入时，首次唤醒GameObject时激活所有的Components
			// 从存档读入时，Component的标记_awaked被读入，不会重复激活
			AttachComponents();
			// m_GameObject._OnAwake += newDelegate(this, &ExtData::AttachComponents);
};

		~ExtData() override
		{
#ifdef DEBUG_COMPONENT
			Debug::Log("[%s]%s [%s]%s  calling GameObject [%s]%s destroy all component.\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str(), m_GameObject.thisName.c_str(), m_GameObject.thisId.c_str());
#endif // DEBUG
			// delete *m_GameObject;
			m_GameObject.ForeachChild([](Component* c)
				{ c->EnsureDestroy(); });
		};

		virtual std::string GetExtName() override
		{
			return this->ExtName;
		}

		virtual std::string GetBaseName() override
		{
			return this->BaseName;
		}

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override
		{
			if (ptr == this->OwnerObject())
			{
#ifdef DEBUG_COMPONENT
				Debug::Log("[%s]%s [%s]%s Owner is Release, GameObject [%s]%s.\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str(), m_GameObject.thisName.c_str(), m_GameObject.thisId.c_str());
#endif // DEBUG
				_ownerIsRelease = true;
				_status = nullptr;
			}
			m_GameObject.Foreach([&ptr](Component* c)
				{ c->InvalidatePointer(ptr); });
		};

#pragma region Save/Load
		template <typename T>
		void Serialize(T& stream)
		{
			stream
				// 序列化GameObject对象，将调用GameObject的Save/Load函数，
				// 由GameObject自己维护自己的读存档长度
				// GameObject自身储存的列表如何变化，都不影响Ext的读存档
				.Process(this->m_GameObject);
		};

		virtual void LoadFromStream(ExStreamReader& stream) override
		{
			// 首先读取ext
			Extension<TBase>::LoadFromStream(stream);
#ifdef DEBUG_COMPONENT
			Debug::Log("[%s]%s [%s]%s GameObject [%s]%s Load from stream.\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str(), m_GameObject.thisName.c_str(), m_GameObject.thisId.c_str());
#endif // DEBUG
			// 读取GameObject，由GameObject自身清理不用的Component，再读入存档
			this->Serialize(stream);
		};
		virtual void SaveToStream(ExStreamWriter& stream) override
		{
			Extension<TBase>::SaveToStream(stream);
			this->Serialize(stream);
		};
#pragma endregion

		//----------------------
		// GameObject
		virtual GameObject* GetGameObject() override
		{
			return m_GameObject.GetAwaked();
		};
		__declspec(property(get = GetGameObject)) GameObject* _GameObject;

		template <typename TComponent>
		TComponent* GetComponent()
		{
			return m_GameObject.GetComponentInChildren<TComponent>();
		}

		template <typename TStatus>
		TStatus* GetExtStatus()
		{
			if (_status == nullptr && !_ownerIsRelease)
			{
				_status = m_GameObject.GetComponentInChildren<TStatus>();
			}
			return static_cast<TStatus*>(_status);
		}

		void SetExtStatus(Component* pStatus)
		{
			_status = pStatus;
		}
	private:
		//----------------------
		// GameObject
		// GO作为实例进行储存
		GameObject m_GameObject{};

		// Status Component
		Component* _status = nullptr;

		bool _ownerIsRelease = false;

		//----------------------
		// Scripts

		/// <summary>
		///  call by GameObject _OnAwake Event
		/// 逻辑开始运行时再对Component进行实例化
		/// Component的Awake分两种情况调用：
		/// 当正常开始游戏时，TechnoClass_Init触发调用_GameObject，唤醒Components，此时可以获得TechnoType；
		/// 但当从存档载入时，TechnoClass_Init不会触发，而是通过TechnoClass_Load_Suffix实例化TechnoExt，此时无法获得TechnoType，
		/// 在LoadFromStream里读取_awaked，跳过执行Awake()，直接通过LoadFromStream读取数据
		/// </summary>
		void AttachComponents()
		{
#ifdef DEBUG_COMPONENT
			Debug::Log("[%s]%s [%s]%s call AttachComponents\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str());
#endif // DEBUG
			if (m_GlobalScriptsCreated)
			{
				return;
			}
			// Search and instantiate global script objects in TechnoExt
			std::list<Component*> m_GlobalScripts{};

			// 在Ext中创建Component实例并加入到GameObject中
			// Component的创建需要使用new，在Component::EnsureDesroy中Delete
			TExt::AddGlobalScripts(m_GlobalScripts, this);
#ifdef DEBUG_COMPONENT
			Debug::Log("[%s]%s [%s]%s ready to attach %d components\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str(), m_GlobalScripts.size());
#endif // DEBUG
			// 该函数只将Component实例加入GameObject
			for (Component* s : m_GlobalScripts)
			{
				m_GameObject.AddComponent(*s);
			}
			/*for (Component* s : m_GlobalScripts)
			{
				s->EnsureAwaked();
			}*/
			m_GlobalScriptsCreated = true;
		};

		// 全局脚本
		bool m_GlobalScriptsCreated = false;
	};

	class ExtContainer : public Container<TExt>
	{
	public:
		ExtContainer() : Container<TExt>(typeid(TExt).name()) {}
		~ExtContainer() = default;
	};
};
