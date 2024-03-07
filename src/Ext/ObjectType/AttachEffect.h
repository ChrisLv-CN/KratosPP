#pragma once

#include <string>
#include <vector>
#include <map>
#include <type_traits>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/Helper/FLH.h>

#include <Ext/EffectType/AttachEffectData.h>
#include <Ext/EffectType/AttachEffectTypeData.h>

class AttachEffectScript;
class BulletStatus;
class TechnoStatus;

/// @brief AEManager, sub-component is AttachEffectScript, and AttachEffectScript 's sub-component is EffectScript
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class AttachEffect : public ObjectScript
{
public:
	OBJECT_SCRIPT(AttachEffect);

	bool InBuilding();
	bool InSelling();

	bool OwnerIsDead();

	/**
	 *@brief 当前所有AE的数量
	 *
	 * @return int AE数量
	 */
	int Count();

	void GetMarks(std::vector<std::string>& marks);
	std::vector<std::string> GetMarks();
	void GetAENames(std::vector<std::string>& names);
	bool HasStand();

	void AEStateToStand(EffectData* pData, int duration, std::string token, bool resetDuration);

	void ClearLocationMarks();

	/**
	 *@brief 统计所有的AEBuff
	 *
	 * @return CrateBuffData
	 */
	CrateBuffData CountAttachStatusMultiplier();

	/**
	 *@brief 读取所有生效的免疫Buff
	 *
	 * @return ImmuneData
	 */
	ImmuneData GetImmuneData();

	/**
	 *@brief 重设车厢间距
	 *
	 * @param cabinLength 车厢间距
	 */
	void SetLocationSpace(int cabinLength);

	/**
	 *@brief 附加自身携带的AE
	 *
	 * @param typeData AE类型
	 */
	void Attach(AttachEffectTypeData* typeData);

	/**
	 *@brief 按照清单来附加AE
	 *
	 * @param types 清单
	 * @param chances 几率清单
	 * @param onceCheck 检查单次标记
	 * @param pSource 来源
	 * @param pSourceHouse 来源所属
	 * @param warheadLocation 通过弹头附加时弹头的位置
	 * @param aeMode 分组编号
	 * @param fromPassenger 来自乘客
	 */
	void Attach(std::vector<std::string> types, std::vector<double> chances = {}, bool onceCheck = false,
		ObjectClass* pSource = nullptr, HouseClass* pSourceHouse = nullptr,
		CoordStruct warheadLocation = CoordStruct::Empty, int aeMode = -1, bool fromPassenger = false);

	/**
	 *@brief 按照AE的Section来附加
	 *
	 * @param type section名称
	 * @param onceCheck 检查单次标记
	 * @param pSource 来源
	 * @param pSourceHouse 来源所属
	 * @param warheadLocation 通过弹头附加时弹头的位置
	 * @param aeMode 分组编号
	 * @param fromPassenger 来自乘客
	 */
	void Attach(std::string type, bool onceCheck = false,
		ObjectClass* pSource = nullptr, HouseClass* pSourceHouse = nullptr,
		CoordStruct warheadLocation = CoordStruct::Empty, int aeMode = -1, bool fromPassenger = false);

	/**
	 *@brief 附加一个AE
	 *
	 * @param data AE类型
	 * @param pSource 来源
	 * @param pSourceHouse 来源所属
	 * @param warheadLocation 通过弹头附加时弹头的位置
	 * @param aeMode 分组编号
	 * @param fromPassenger 来自乘客
	 */
	void Attach(AttachEffectData data,
		ObjectClass* pSource = nullptr, HouseClass* pSourceHouse = nullptr,
		CoordStruct warheadLocation = CoordStruct::Empty, int aeMode = -1, bool fromPassenger = false);

	/**
	 *@brief 由自身武器发射赋予自身AE
	 *
	 * @param pWeapon 武器
	 */
	void FeedbackAttach(WeaponTypeClass* pWeapon);

	/**
	 *@brief 关闭并移除一组AE
	 *
	 * @param aeTypes AE的名称
	 */
	void DetachByName(std::vector<std::string> aeTypes);

	/**
	 *@brief 只关闭一定数量的AE
	 *
	 * @param aeTypes
	 */
	void DetachByName(std::map<std::string, int> aeTypes);

	/**
	 *@brief 关闭并移除一组AE
	 *
	 * @param aeTypes AE的名称
	 */
	void DetachByMarks(std::vector<std::string> marks);

	/**
	 *@brief 关闭并移除一个AE
	 *
	 * @param token 令牌
	 */
	void DetachByToken(std::string token);

	/**
	 * @brief 检查生命值并关闭死亡的AE
	 *
	 * @param silence 沉默模式，只移除，不触发AE结束事件、冷却计时和Next附加
	 */
	void CheckDurationAndDisable(bool silence = false);

	void OnGScreenRender(EventSystem* sender, Event e, void* args);

	virtual void Awake() override
	{
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
	}

	virtual void ExtChanged() override
	{
		_typeData = nullptr;
		_groupData = nullptr;
		_attachOnceFlag = false;
	}

	virtual void Destroy() override
	{
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
		((TechnoExt::ExtData*)_extData)->SetExtStatus(nullptr);
	}

	virtual void OnUpdate() override;
	virtual void OnWarpUpdate() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;
	virtual void OnRemove() override;

	virtual void CanFire(AbstractClass* pTarget, WeaponTypeClass* pWeapon, bool& ceaseFire) override;
	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

	virtual void OnReceiveDamageDestroy() override;

	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) override;

	virtual void OnUnInit() override;

	bool PowerOff = false; // 停电状态

	std::vector<int> PassengerIds{}; // 乘客持有的AEMode ID
	std::map<std::string, CDTimerClass> DisableDelayTimers{};
	std::map<std::string, int> AEStacks{};

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->PowerOff)
			.Process(this->PassengerIds)

			.Process(this->DisableDelayTimers)
			.Process(this->AEStacks)

			.Process(this->_ownerIsDead)
			.Process(this->_attachOnceFlag)

			.Process(this->_location)
			.Process(this->_lastLocation)
			.Process(this->_locationMarks)
			.Process(this->_locationMarkDistance)
			.Process(this->_totalMileage)
			.Process(this->_locationSpace)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<AttachEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	BulletStatus* GetBulletStatus();
	TechnoStatus* GetTechnoStatus();

	/**
	 *@brief 赋予由乘客带来的AE
	 *
	 */
	void AttachUploadAE();

	/**
	 *@brief 赋予自身的多组AE，读取乘客身上的AEMode来赋予指定的一组AE
	 *
	 */
	void AttachGroupAE();

	void AttachStateEffect();

	bool IsOnMark(FilterData data);
	bool HasContradiction(AttachEffectData data);

	bool IsOnDelay(AttachEffectData data);
	void StartDelay(AttachEffectData data);

	void AddStackCount(AttachEffectData data);
	void ReduceStackCount(AttachEffectData data);
	bool StackNotFull(AttachEffectData data);
	/**
	 *@brief AE堆叠时，获取AE的堆叠位置偏移
	 *
	 * @param aeData AE类型
	 * @param offsetData 偏移设置
	 * @param offsetMarks 没有分组的堆叠，以AE名字为索引，取第一个AE的位置做偏移
	 * @param groupMarks 有分组的堆叠，以分组为索引，取第一个分组的位置做偏移
	 * @param groupFirstMarks 有分组的堆叠，以分组为索引，记录每个组的第一个偏移位置
	 * @return Offset 当前AE的偏移值
	 */
	CoordStruct StackOffset(AttachEffectData aeData, OffsetData offsetData,
		std::map<std::string, CoordStruct>& offsetMarks,
		std::map<int, CoordStruct>& groupMarks,
		std::map<int, CoordStruct>& groupFirstMarks);

	/**
	 *@brief 根据火车的位置，获取插入的序号
	 *
	 * @param data AE类型
	 * @return int 序号
	 */
	int FindInsertIndex(AttachEffectData data);
	/**
	 *@brief 更新火车替身车厢的位置
	 *
	 * @param ae AE
	 * @param markIndex 记录的序号
	 * @return true
	 * @return false
	 */
	bool UpdateTrainStandLocation(AttachEffectScript* ae, int& markIndex);
	CoordStruct MarkLocation();

	bool _ownerIsDead = false;

	/**
	 *@brief 已经执行过一次Update赋予的标记，对于AttachOnce的AE来说，下一次不会再赋予
	 *
	 */
	bool _attachOnceFlag = false;

	/**
	 *@brief 部分AE以状态方式写在TechnoType标签里，在初始化时，自动附加一个AE
	 *
	 */
	bool _attachStateEffectFlag = false;

	CoordStruct _location{}; // 当前位置
	CoordStruct _lastLocation{}; // 上一次位置
	std::vector<LocationMark> _locationMarks{}; // 位置记录
	int _locationMarkDistance = 16; // 多少格记录一个位置
	double _totalMileage = 0; // 总里程
	int _locationSpace = 512; // 替身火车的车厢间距

	// section上的AE设置
	AttachEffectTypeData* _typeData = nullptr;
	AttachEffectTypeData* GetTypeData();

	AttachEffectGroupData* _groupData = nullptr;
	AttachEffectGroupData* GetGroupData();
};
