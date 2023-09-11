#pragma once

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
		ExtData(TBase* OwnerObject) : Extension<TBase>(OwnerObject)
		{
#ifdef DEBUG
			char t_this[1024];
			sprintf_s(t_this, "%p", this);
			this->thisId = { t_this };

			this->thisName = typeid(TExt).name();

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
			// 为GameObject添加激活事件，附加Components.
			// 不从存档读入时，首次唤醒GameObject时激活Components
			// 从存档读入时，Component的标记_awaked被读入，不会重复激活
			m_GameObject._OnAwake += newDelegate(this, &ExtData::AttachComponents);
		};

		~ExtData() override
		{
			m_GameObject.EnsureDestroy();
			// delete *m_GameObject;
			// m_GameObject.release();
		};

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override
		{
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
#ifdef DEBUG
			Debug::Log("[%s]%s [%s]%s GameObject [%s]%s Load from stream.\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str(), m_GameObject.thisName.c_str(), m_GameObject.thisId.c_str());
#endif // DEBUG
			// 准备Component
			AttachComponents();
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
		GameObject* GetGameObject()
		{
			return m_GameObject.GetAwaked();
		};
		__declspec(property(get = GetGameObject)) GameObject* _GameObject;

	private:
		//----------------------
		// GameObject
		// GO作为实例进行储存
		GameObject m_GameObject{};

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
#ifdef DEBUG
			Debug::Log("[%s]%s [%s]%s call AttachComponents\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str());
#endif // DEBUG
			if (m_GlobalScriptsCreated)
			{
				return;
			}
			// Search and instantiate global script objects in TechnoExt
			std::list<Component*> m_GlobalScripts{};
			TExt::AddGlobalScripts(&m_GlobalScripts, this);
#ifdef DEBUG
			Debug::Log("[%s]%s [%s]%s ready to attach %d components\n", this->thisName.c_str(), this->thisId.c_str(), this->baseName.c_str(), this->baseId.c_str(), m_GlobalScripts.size());
#endif // DEBUG
			// 该函数只将Component实例加入GameObject
			for (Component* s : m_GlobalScripts)
			{
				m_GameObject.AddComponentNotAwake(s);
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
