#pragma once

#include <string>

#include <Extension.h>
#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/AnimExt.h>
#include <Ext/BulletStatus.h>
#include <Ext/TechnoStatus.h>

#include <Ext/AnimType/AnimDamageData.h>
#include <Ext/AnimType/ExpireAnimData.h>
#include <Ext/AnimType/PlaySuperData.h>
#include <Ext/AnimType/SpawnAnimsData.h>
#include <Ext/Data/OffsetData.h>
#include <Ext/Data/RelationData.h>
#include <Ext/State/PaintballData.h>

class AnimStatus : public AnimScript
{
public:
	ANIM_SCRIPT(AnimStatus);

	bool TryGetCreater(TechnoClass*& pTechno);

	void SetOffset(OffsetData data);

	void UpdateVisibility(Relation visibility);

	/**
	 *@brief 接管伤害制造
	 * @param isBounce 是否流星，碎片
	 * @param bright 弹头闪光
	*/
	void Explosion_Damage(bool isBounce = false, bool bright = false);

	/**
	 * @brief 替换流星、碎片击中水中的动画
	 *
	 * @return true
	 * @return false
	 */
	bool OverrideExpireAnimOnWater();

	/**
	 *@brief 染色
	 *
	 * @param R
	 */
	void DrawSHP_Paintball(REGISTERS* R);

	virtual void OnUpdate() override;

	virtual void OnLoop() override;

	virtual void OnDone() override;

	virtual void OnNext(AnimTypeClass* pNext) override;

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

			.Process(this->_initDamageDelayFlag)
			.Process(this->_damageDelayTimer)
			.Process(this->_createrIsDeadth)

			.Process(this->_offsetData)
			.Process(this->_initInvisibleFlag)
			.Process(this->_playSuperFlag)

			.Process(this->_initSpawnFlag)
			.Process(this->_spawnInitDelayTimer)
			.Process(this->_spawnDelayTimer)
			.Process(this->_spawnCount)
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
	bool GetInvisible(Relation visibility);

	void ResetLoopSpawn();

	void OnUpdate_Visibility();
	void OnUpdate_Damage();
	void OnUpdate_SpawnAnims();
	void OnUpdate_PlaySuper();

	void OnLoop_SpawnAnims();
	void OnLoop_PlaySuper();

	void OnDone_SpawnAnims();
	void OnDone_PlaySuper();

	void OnNext_SpawnAnims(AnimTypeClass* pNext);

	OffsetData _offsetData{};

	bool _initDamageDelayFlag = false;
	CDTimerClass _damageDelayTimer{};

	bool _createrIsDeadth = false;

	bool _initInvisibleFlag = false;
	bool _playSuperFlag = false;

	bool _initSpawnFlag = false;
	CDTimerClass _spawnInitDelayTimer{};
	CDTimerClass _spawnDelayTimer{};
	int _spawnCount = 0;

	AnimDamageData* _animDamageData = nullptr;
	AnimDamageData* GetAnimDamageData();

	ExpireAnimData* _expireAnimData = nullptr;
	ExpireAnimData* GetExpireAnimData();

	PaintballData* _paintballData = nullptr;
	PaintballData* GetPaintballData();

	PlaySuperData* _playSuperData = nullptr;
	PlaySuperData* GetPlaySuperData();

	SpawnAnimsData* _spawnAnimsData = nullptr;
	SpawnAnimsData* GetSpawnAnimsData();
};
