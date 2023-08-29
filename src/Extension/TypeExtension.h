#pragma once

#include <typeinfo>
#include <stack>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>

template <typename TBase, typename TExt>
class TypeExtension
{
public:
	using base_type = TBase;

	class ExtData : public Extension<TBase>
	{
	public:
		ExtData(TBase* OwnerObject) : Extension<TBase>(OwnerObject) { };

		virtual ~ExtData() override { };

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { };

		virtual void Initialize() override
		{

		};

		virtual void LoadFromINIFile(CCINIClass* pINI) override
		{
			auto pThis = this->OwnerObject();
			const char* pSection = pThis->ID;

			if (!pINI->GetSection(pSection))
			{
				return;
			}

			INI_EX exINI(pINI);

			//TODO read ini
		};

		virtual void LoadFromStream(ExStreamReader& Stm) override
		{
			Extension<TBase>::LoadFromStream(Stm);
			this->Serialize(Stm);
		};

		virtual void SaveToStream(ExStreamWriter& Stm) override
		{
			Extension<TBase>::SaveToStream(Stm);
			this->Serialize(Stm);
		};

	private:
		template <typename T>
		void Serialize(T& Stm)
		{

		};
	};

	class ExtContainer : public Container<TExt>
	{
	public:
		ExtContainer() : Container<TExt>(typeid(TExt).name()) { };
		~ExtContainer() = default;
	};

};

