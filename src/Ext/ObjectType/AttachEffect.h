﻿#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <type_traits>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/Helper/FLH.h>

#include <Ext/EffectType/AttachEffectData.h>

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

	bool PowerOff; // 停电状态

	std::vector<int> PassengerIds{}; // 乘客持有的AEMode ID

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
	void GetAENames(std::vector<std::string>& names);

	/**
	 *@brief 重设车厢间距
	 *
	 * @param cabinLength 车厢间距
	 */
	void SetLocationSpace(int cabinLength);

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
	void Attach(AttachEffectData data, ObjectClass* pSource, HouseClass* pSourceHouse = nullptr, CoordStruct warheadLocation = CoordStruct::Empty, int aeMode = -1, bool fromPassenger = false);

	virtual void Awake() override;

	virtual void Destroy() override;

	void OnGScreenRender(EventSystem* sender, Event e, void* args);

	virtual void OnReceiveDamageDestroy();

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->_ownerIsDead)

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
	bool IsOnMark(AttachEffectData data);
	bool HasContradiction(AttachEffectData data);

	void AddStackCount(AttachEffectData data);
	void ReduceStackCount(AttachEffectData data);
	bool StackNotFull(AttachEffectData data);

	/**
	 *@brief 根据火车的位置，获取插入的序号
	 *
	 * @param data AE类型
	 * @return int 序号
	 */
	int FindInsertIndex(AttachEffectData data);

	bool _ownerIsDead = false;

	bool _attachEffectOnceFlag = false;

	CoordStruct _location{}; // 当前位置
	CoordStruct _lastLocation{}; // 上一次位置
	std::vector<LocationMark> _locationMarks{}; // 位置记录
	int _locationMarkDistance = 16; // 多少格记录一个位置
	double _totalMileage = 0; // 总里程
	int _locationSpace = 512; // 替身火车的车厢间距
};
