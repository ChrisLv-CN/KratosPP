#pragma once
#include <TechnoTypeClass.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>
#include "GOExtension.h"

class Matrix3D;

class TechnoTypeExt
{
public:
	using base_type = TechnoTypeClass;

	static constexpr DWORD Canary = 0x11111111;
	static constexpr size_t ExtPointerOffset = 0xDF4;

	class ExtData final : public Extension<TechnoTypeClass>
	{
	public:
		ExtData(TechnoTypeClass* OwnerObject) : Extension<TechnoTypeClass>(OwnerObject)
		{ }

		virtual ~ExtData() = default;
		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual void Initialize() override;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;
	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<TechnoTypeExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;
};
