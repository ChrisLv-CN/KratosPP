#pragma once

#include <typeinfo>

#include <Common/Components/GameObject.h>
#include <Ext/LaserTrail.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>

#include <TechnoClass.h>

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
			, _GameObject(goName)
			, _LaserTrail(OwnerObject)
		{
			_GameObject.AddComponent(&_LaserTrail);
			_GameObject.Foreach([](Component* c) {c->Awake(); });
		}

		~ExtData() = default;

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

		std::string goName = typeid(TExt).name();
		GameObject _GameObject;
		LaserTrail _LaserTrail;
	private:
		template <typename T>
		void Serialize(T& Stm)
		{
			//_GameObject.Foreach([](Component* c) {c->Serialize<T>(Stm); });
		};

	};

	class ExtContainer : public Container<TExt>
	{
	public:
		ExtContainer() : Container<TExt>(typeid(TExt).name())
		{ }
		~ExtContainer() = default;
	};
};
