#pragma once

#include <string>

#include <Extension.h>
#include <TechnoClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/AnimExt.h>
#include <Ext/BulletStatus.h>
#include <Ext/TechnoStatus.h>
#include <Ext/Data/OffsetData.h>
#include <Ext/State/PaintballData.h>

class AnimStatus : public AnimScript
{
public:
	AnimStatus(AnimExt::ExtData* ext) : AnimScript(ext)
	{
		this->Name = typeid(this).name();
	}

	bool TryGetCreater(TechnoClass*& pTechno);

	void SetOffset(OffsetData data);

	/**
	 *@brief 接管伤害制造
	 * @param isBounce 是否流星，碎片
	 * @param bright 弹头闪光
	*/
	void Explosion_Damage(bool isBounce = false, bool bright = false) {};

	/**
	 * @brief 替换流星、碎片击中水中的动画
	 *
	 * @return true
	 * @return false
	 */
	bool OverrideExpireAnimOnWater() { return false; };

	/**
	 *@brief 染色
	 *
	 * @param R
	 */
	void DrawSHP_Paintball(REGISTERS* R);

	virtual void OnUpdate() override;

	void OnUpdate_Visibility();
	void OnUpdate_Damage();
	void OnUpdate_SpawnAnims();

	virtual void OnLoop() override;

	void OnLoop_SpawnAnims();

	virtual void OnDone() override;

	void OnDone_SpawnAnims();

	virtual void OnNext(AnimTypeClass* pNext) override;

	void OnNext_SpawnAnims(AnimTypeClass* pNext);

	TechnoClass* pCreater = nullptr;
	TechnoClass* pAttachOwner = nullptr; // 动画附着的对象
	CoordStruct Offset = CoordStruct::Empty; // 附着的偏移位置

	virtual void InvalidatePointer(void* ptr) override
	{
		AnnounceInvalidPointer(this->pCreater, ptr);
	};

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pCreater)
			.Process(this->pAttachOwner)
			.Process(this->Offset)
			.Process(this->_offsetData)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<AnimStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	OffsetData _offsetData{};

	PaintballData* _paintballData = nullptr;
	PaintballData* GetPaintballData();
};
