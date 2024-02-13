#include "AttachEffect.h"

#include <BuildingClass.h>
#include <MissionClass.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Common/PrintTextManager.h>

#include <Ext/EffectType/AttachEffectScript.h>
#include <Ext/TechnoType/Status/UploadAttachData.h>
#include <Ext/WeaponType/FeedbackAttachData.h>

OBJECT_SCRIPT_CPP(AttachEffect);

bool AttachEffect::OwnerIsDead()
{
	if (!_ownerIsDead)
	{
		if (pBullet && IsDead(pBullet))
		{
			_ownerIsDead = true;
		}
		else if (pTechno && IsDead(pTechno))
		{
			_ownerIsDead = true;
		}
	}
	return _ownerIsDead;
}

bool AttachEffect::InBuilding()
{
	if (IsBuilding() && !OwnerIsDead())
	{
		BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(pTechno);
		return pBuilding->BState == BStateType::Construction && pBuilding->CurrentMission == Mission::Selling;
	}
	return false;
}

bool AttachEffect::InSelling()
{
	if (IsBuilding())
	{
		BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(pTechno);
		return pBuilding->BState == BStateType::Construction && pBuilding->CurrentMission == Mission::Selling && pBuilding->MissionStatus > 0;
	}
	return false;
}

int AttachEffect::Count()
{
	return _children.size();
}

void AttachEffect::GetMarks(std::vector<std::string>& marks)
{
	ForeachChild([&marks](Component* c) {
		if (auto ae = dynamic_cast<AttachEffectScript*>(c)) {
			ae->GetMarks(marks);
		}
		});
}

void AttachEffect::GetAENames(std::vector<std::string>& names)
{
	ForeachChild([&names](Component* c) {
		if (auto ae = dynamic_cast<AttachEffectScript*>(c)) {
			names.push_back(ae->AEData.Name);
		}
		});
}

void AttachEffect::SetLocationSpace(int cabinLength)
{
	_locationSpace = cabinLength;
	if (cabinLength < _locationMarkDistance)
	{
		_locationMarkDistance = cabinLength;
	}
}

void AttachEffect::Attach(AttachEffectTypeData* typeData)
{
	if (typeData->Enable)
	{
		Attach(typeData->AttachEffectTypes, {}, _attachOnceFlag, pObject);
	}
	if (typeData->StandTrainCabinLength > 0)
	{
		SetLocationSpace(typeData->StandTrainCabinLength);
	}

}

void AttachEffect::Attach(std::vector<std::string> types, std::vector<double> chances, bool onceCheck,
	ObjectClass* pSource, HouseClass* pSourceHouse,
	CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	if (!types.empty())
	{
		int index = 0;
		for (std::string type : types)
		{
			if (Bingo(chances, index))
			{
				Attach(type, onceCheck, pSource, pSourceHouse, warheadLocation, aeMode, fromPassenger);
			}
			index++;
		}
	}
}

void AttachEffect::Attach(std::string type, bool onceCheck,
	ObjectClass* pSource, HouseClass* pSourceHouse,
	CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	if (IsNotNone(type))
	{
		AttachEffectData* data = INI::GetConfig<AttachEffectData>(INI::Rules, type.c_str())->Data;
		if (data->Enable)
		{
			if (onceCheck && data->AttachOnceInTechnoType)
			{
				return;
			}
			Attach(*data, pSource, pSourceHouse, warheadLocation, aeMode, fromPassenger);
		}
	}
}

void AttachEffect::Attach(AttachEffectData data,
	ObjectClass* pSource, HouseClass* pSourceHouse,
	CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	if (!data.Enable)
	{
		Debug::Log("[Developer warning]Attemp to attach an invalid AE [%s] to [%s]\n", data.Name.c_str(), pObject->GetType()->ID);
		return;
	}
	// 检查是否穿透铁幕
	if (!data.PenetratesIronCurtain && pObject->IsIronCurtained())
	{
		return;
	}
	// 是否在白名单上
	if (!data.CanAffectType(pObject))
	{
		return;
	}
	// 是否需要标记
	if (!IsOnMark(data))
	{
		return;
	}
	// 是否有排斥的AE
	if (HasContradiction(data))
	{
		return;
	}

	TechnoClass* pAttacker = nullptr;
	HouseClass* pAttackingHouse = pSourceHouse;
	// 调整所属
	if (!pSource)
	{
		pSource = pObject;
	}
	TechnoClass* pSourceTechno = nullptr;
	BulletClass* pSourceBullet = nullptr;
	if (CastToTechno(pSource, pSourceTechno))
	{
		pAttacker = pSourceTechno;
		if (!pAttackingHouse)
		{
			pAttackingHouse = pAttacker->Owner;
		}
	}
	else if (CastToBullet(pSource, pSourceBullet))
	{
		pAttacker = pSourceBullet->Owner;
		if (!pAttackingHouse)
		{
			pAttackingHouse = GetSourceHouse(pSourceBullet);
		}
	}
	else
	{
		Debug::Log("[Developer warning]Attach AE [%s] to [%s] form a unknow source [%s]\n", data.Name.c_str(), pObject->GetType()->ID, pSource->WhatAmI());
		return;
	}
	// 更改所属，如果需要
	if (data.ReceiverOwn && pSource != pObject)
	{
		// 所属设置为接受者
		if (pSourceTechno)
		{
			pAttackingHouse = pSourceTechno->Owner;
		}
		else if (pSourceBullet)
		{
			pAttackingHouse = GetSourceHouse(pSourceBullet);
		}
	}
	// 调整攻击者
	if (!fromPassenger && data.FromTransporter && !IsDead(pAttacker))
	{
		pAttacker = WhoIsShooter(pAttacker);
	}
	// 检查叠加
	bool add = data.Cumulative == CumulativeMode::YES;
	if (!add)
	{
		// 不同攻击者是否叠加
		bool isAttackMark = fromPassenger || data.Cumulative == CumulativeMode::ATTACKER && !pAttacker && pAttacker->IsAlive;
		// 不同所属是否叠加
		bool isHouseMark = data.Cumulative == CumulativeMode::HOUSE;
		// 攻击者标记AE名称相同，但可以来自不同的攻击者，可以叠加，不检查Delay
		// 检查冷却计时器
		if (!isAttackMark && !isHouseMark && IsOnDelay(data))
		{
			return;
		}
		bool find = false;
		CoordStruct location = _location;
		// 检查持续时间，增减Duration
		ForeachChild([&find, &add, &isAttackMark, &isHouseMark, &data, &pAttacker, &pAttackingHouse, &location](Component* c) {
			auto temp = dynamic_cast<AttachEffectScript*>(c);
			if (temp && temp->IsActive())
			{
				if (data.Group < 0)
				{
					// 无分组，攻击者标记叠加，或同名重置计时器
					if (temp->AEData.Name == data.Name)
					{
						find = true;
						if (isAttackMark)
						{
							if (temp->pSource == pAttacker)
							{
								// 相同的攻击者，重置持续时间，并跳出循环
								if (temp->AEData.ResetDurationOnReapply)
								{
									temp->ResetDuration();
								}
								c->Break();
								return;
							}
							else
							{
								// 当前条的攻击者不同，设置标记后，继续循环，直到检查完所有的AE
								find = false;
							}
						}
						else if (isHouseMark)
						{
							if (temp->pSourceHouse == pAttackingHouse)
							{
								// 是所属标记，且所属相同，重置持续时间，并跳出循环
								if (temp->AEData.ResetDurationOnReapply)
								{
									temp->ResetDuration();
								}
								c->Break();
								return;
							}
							else
							{
								// 当前条的攻击者不同，设置标记后，继续循环，直到检查完所有的AE
								find = false;
							}
						}
						else
						{
							// 不是标记，重置已存在的AE的持续时间，跳出循环
							if (temp->AEData.ResetDurationOnReapply)
							{
								temp->ResetDuration();
							}
							c->Break();
							return;
						}
					}
				}
				else
				{
					// 有分组，替换或者调整持续时间
					if (temp->IsSameGroup(data))
					{
						// 找到了同组
						find = true;
						if (data.OverrideSameGroup)
						{
							// 执行替换操作，关闭所有的同组AE
							temp->End(location);
							add = true;
							// 继续循环直至全部关闭
						}
						else
						{
							// 调整持续时间
							temp->MergeDuration(data.Duration);
							// 继续循环直至全部调整完
						}
					}
				}
			}
			});
		// 没找到同类或同组，可以添加新的实例
		add = add || !find;
	}
	// 可以添加AE，开始执行添加动作
	if (add && data.GetDuration() != 0 && StackNotFull(data))
	{
		int index = FindInsertIndex(data);
		Component* c = AddComponent(AttachEffectScript::ScriptName, index); // 插队
		if (c)
		{
			AddStackCount(data); // 叠层计数
			// 初始化AE
			AttachEffectScript* ae = dynamic_cast<AttachEffectScript*>(c);
			ae->AEData = data;
			// 激活AE
			ae->EnsureAwaked();
			ae->Start(pAttacker, pAttackingHouse, warheadLocation, aeMode, fromPassenger);
		}

	}
}

void AttachEffect::FeedbackAttach(WeaponTypeClass* pWeapon)
{
	// Feedback
	if (pWeapon && !IsDeadOrInvisible(pObject))
	{
		FeedbackAttachData* typeData = INI::GetConfig<FeedbackAttachData>(INI::Rules, pWeapon->ID)->Data;
		if (typeData->Enable)
		{
			bool inTransporter = pTechno && pTechno->Transporter != nullptr;
			TechnoClass* pTransporter = nullptr;
			AttachEffect* pTransporterAEM = nullptr;
			if (inTransporter)
			{
				pTransporter = WhoIsShooter(pTechno);
				pTransporterAEM = GetAEManager<TechnoExt>(pTransporter);
			}
			std::map<int, FeedbackAttachEntity> aeTypes = typeData->Datas;
			for (auto it = aeTypes.begin(); it != aeTypes.end(); it++)
			{
				FeedbackAttachEntity data = it->second;
				if (data.Enable)
				{
					// 检查所属是否平民
					if (pTechno && data.DeactiveWhenCivilian && IsCivilian(pTechno->Owner))
					{
						continue;
					}
					if (pTechno)
					{
						if (data.AffectTechno)
						{
							TechnoClass* tempTechno = pTechno;
							AttachEffect* tempAEM = this;
							// 载具内且能赋予载具，则赋予，不能则跳过
							if (inTransporter)
							{
								if (!pTransporterAEM)
								{
									continue;
								}
								tempTechno = pTransporter;
								tempAEM = pTransporterAEM;
							}
							// 检查是否可以影响并赋予AE
							if (data.CanAffectType(tempTechno)
								&& (data.AffectInAir || !tempTechno->InAir)
								// && (e.AffectStand || !AmIStand(tempTechno))
								&& tempAEM->IsOnMark(data)
								)
							{
								tempAEM->Attach(data.AttachEffects, data.AttachChances);
							}
						}
					}
					else if (data.AffectBullet && pBullet)
					{
						if (data.CanAffectType(pBullet) && IsOnMark(data))
						{
							Attach(data.AttachEffects, data.AttachChances);
						}
					}
				}
			}
		}
	}
}

void AttachEffect::AttachUploadAE()
{
	// 读取所有的乘客，获取Id清单，并依据乘客的设置，为载具添加AE
	PassengerIds.clear();
	if (pTechno->Passengers.NumPassengers > 0)
	{
		// 有乘客
		ObjectClass* pPassenger = pTechno->Passengers.FirstPassenger;
		do
		{
			TechnoClass* pPT = dynamic_cast<TechnoClass*>(pPassenger);
			if (!IsDead(pPT))
			{
				// 查找乘客身上的AEMode设置
				AttachEffect* aeManager = nullptr;
				if (TryGetAEManager<TechnoExt>(pPT, aeManager))
				{
					int aeMode = aeManager->GetTypeData()->AEMode;
					if (aeMode >= 0)
					{
						PassengerIds.push_back(aeMode);
					}
				}
				// 查找乘客身上的AE设置，赋予载具
				UploadAttachData* uploadData = INI::GetConfig<UploadAttachData>(INI::Rules, pPT->GetType()->ID)->Data;
				if (uploadData->Enable)
				{
					std::map<int, UploadAttachEntity> uploadDatas = uploadData->Datas;
					for (auto it = uploadDatas.begin(); it != uploadDatas.end(); it++)
					{
						UploadAttachEntity e = it->second;
						if (e.Enable
							&& e.CanAffectType(pTechno)
							&& (e.AffectInAir || !pTechno->InAir)
							// && (e.AffectStand || !AmIStand(pTechno))
							&& this->IsOnMark(e)
							)
						{
							Attach(e.AttachEffects, {}, false, pPT, nullptr, CoordStruct::Empty, -1, e.SourceIsPassenger);
						}
					}
				}
			}
		} while (pPassenger = pPassenger->NextObject);
	}
}

void AttachEffect::AttachGroupAE()
{
	if (GetGroupData()->Enable)
	{
		std::map<int, AttachEffectTypeData> groupData = GetGroupData()->Datas;
		for (auto it = groupData.begin(); it != groupData.end(); it++)
		{
			AttachEffectTypeData aeType = it->second;
			if (aeType.AttachByPassenger)
			{
				// 该组AE需要乘客进行激活
				int aeMode = aeType.AEModeIndex;
				auto ite = std::find(PassengerIds.begin(), PassengerIds.end(), aeMode);
				if (ite != PassengerIds.end())
				{
					// 乘客中有这个ID
					Attach(aeType.AttachEffectTypes, {}, false, pObject, nullptr, CoordStruct::Empty, aeMode, false);
				}
			}
			else
			{
				// 该组AE不需要乘客激活，直接赋予
				Attach(&aeType);
			}
		}
	}
}

void AttachEffect::CheckDurationAndDisable()
{
	CoordStruct location = _location;
	ForeachChild([&location, this](Component* c) {
		AttachEffectScript* ae = dynamic_cast<AttachEffectScript*>(c);
		// 执行IsAlive时，检查AE的生命状态，失效的AE会在这里被标记为Deactivate
		if (ae && !ae->IsAlive())
		{
			AttachEffectData data = ae->AEData;
			// 加入冷却计时器
			this->StartDelay(data);
			// 结束AE
			ae->End(location);
			// Deactivate的组件不会再执行Foreach事件，标记为失效，以便父组件将其删除
			ae->Disable();
			this->ReduceStackCount(data);
			// 添加NextAE
			std::string nextAE = data.Next;
			if (IsNotNone(nextAE) && IsDeadOrInvisible(pObject))
			{
				Attach(nextAE, false, ae->pSource, ae->pSourceHouse);
			}
		}
		});
}

AttachEffectTypeData* AttachEffect::GetTypeData()
{
	if (!_typeData)
	{
		_typeData = INI::GetConfig<AttachEffectTypeData>(INI::Rules, pObject->GetType()->ID)->Data;
	}
	return _typeData;
}

AttachEffectGroupData* AttachEffect::GetGroupData()
{
	if (!_groupData)
	{
		_groupData = INI::GetConfig<AttachEffectGroupData>(INI::Rules, pObject->GetType()->ID)->Data;
	}
	return _groupData;
}

bool AttachEffect::IsOnMark(FilterData data)
{
	std::vector<std::string> marks;
	GetMarks(marks);
	return data.IsOnMark(marks);
}

bool AttachEffect::HasContradiction(AttachEffectData data)
{
	std::vector<std::string> names;
	GetAENames(names);
	return data.HasContradiction(names);
}

bool AttachEffect::IsOnDelay(AttachEffectData data)
{
	std::string name = data.Name;
	auto it = _disableDelayTimers.find(name);
	if (it != _disableDelayTimers.end())
	{
		return it->second.InProgress();
	}
	return false;
}

void AttachEffect::StartDelay(AttachEffectData data)
{
	std::string name = data.Name;
	auto it = _disableDelayTimers.find(name);
	if (it == _disableDelayTimers.end() || it->second.Expired())
	{
		int delay = GetRandomValue(data.RandomDelay, data.Delay);
		if (delay > 0)
		{
			CDTimerClass timer{ delay };
			_disableDelayTimers[name] = timer;
		}
	}
}

void AttachEffect::AddStackCount(AttachEffectData data)
{
	std::string name = data.Name;
	auto it = _aeStacks.find(name);
	if (it != _aeStacks.end())
	{
		it->second++;
	}
	else
	{
		_aeStacks[name] = 1;
	}
}

void AttachEffect::ReduceStackCount(AttachEffectData data)
{
	std::string name = data.Name;
	auto it = _aeStacks.find(name);
	if (it != _aeStacks.end())
	{
		it->second--;
		if (it->second < 1)
		{
			_aeStacks.erase(it);
		}
	}
}

bool AttachEffect::StackNotFull(AttachEffectData data)
{
	if (data.MaxStack > 0)
	{
		std::string name = data.Name;
		auto it = _aeStacks.find(name);
		if (it != _aeStacks.end())
		{
			return it->second < data.MaxStack;
		}
	}
	return true;
}

CoordStruct AttachEffect::StackOffset(AttachEffectData aeData, OffsetData offsetData,
	std::map<std::string, CoordStruct>& offsetMarks,
	std::map<int, CoordStruct>& groupMarks,
	std::map<int, CoordStruct>& groupFirstMarks)
{
	CoordStruct offset = offsetData.Offset;
	if (offsetData.StackGroup > -1)
	{
		// 分组堆叠
		int stackGroup = offsetData.StackGroup;
		auto it = groupMarks.find(stackGroup);
		if (it != groupMarks.end())
		{
			// 有记录，往上堆叠
			CoordStruct offsetMark = it->second;
			offset = offsetMark + offsetData.StackOffset;
			it->second = offset;
		}
		else
		{
			// 没有记录，取最后一个组的初始偏移位置，加上组偏移
			if (!groupFirstMarks.empty())
			{
				auto ite = groupFirstMarks.end();
				ite--;
				offset = ite->second + offsetData.StackGroupOffset;
			}
			// 创建新的分组
			groupMarks[stackGroup] = offset;
			groupFirstMarks[stackGroup] = offset;
		}
	}
	else if (!offsetData.StackOffset.IsEmpty())
	{
		// 无分组堆叠
		std::string aeName = aeData.Name;
		auto it = offsetMarks.find(aeName);
		if (it != offsetMarks.end())
		{
			// 需要进行偏移
			CoordStruct offsetMark = it->second;
			offset = offsetMark + offsetData.StackOffset;
			it->second = offset;
		}
		else
		{
			offsetMarks[aeName] = offset;
		}

	}
	return offset;
}

int AttachEffect::FindInsertIndex(AttachEffectData data)
{
	// TODO 火车插位
	return -1;
}

CoordStruct AttachEffect::MarkLocation()
{
	CoordStruct location = pObject->GetCoords();
	if (_lastLocation.IsEmpty())
	{
		_lastLocation = location;
	}
	else
	{
		double mileage = location.DistanceFrom(_lastLocation);
		if (!isnan(mileage) && mileage > _locationMarkDistance)
		{
			_lastLocation = location;
			double tempMileage = _totalMileage + mileage;
			// 记录下当前的位置
			OffsetData offset{};
			LocationMark mark = GetRelativeLocation(pObject, offset);
			// 插入队头
			_locationMarks.insert(_locationMarks.begin(), mark);
			// 检查容量(当前AE数量+1)，弹出队尾
			if (tempMileage > (Count() + 1) * _locationSpace)
			{
				_locationMarks.pop_back();
			}
			else
			{
				_totalMileage = tempMileage;
			}
		}
	}
	return location;
}

void AttachEffect::ClearLocationMarks()
{
	_locationMarks.clear();
	_lastLocation = CoordStruct::Empty;
	_totalMileage = 0;
}

void AttachEffect::Awake()
{
	EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
}

void AttachEffect::Destroy()
{
	EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
	((TechnoExt::ExtData*)extData)->SetExtStatus(nullptr);
}


void AttachEffect::OnGScreenRender(EventSystem* sender, Event e, void* args)
{
	CoordStruct location = _location;
	if (args)
	{
		// EndRender
		ForeachChild([&location](Component* c) {
			if (auto ae = dynamic_cast<AttachEffectScript*>(c)) { ae->OnGScreenRenderEnd(location); }
			});
#ifdef DEBUG
		int offsetZ = PrintTextManager::GetFontSize().Y;
		// 打印Component结构
		/// GameObject
		///		|__ AttachEffect
		///				|__ AttachEffectScript#0
		///						|__ EffectScript#0
		///						|__ EffectScript#1
		///				|__ AttachEffectScript#1
		///						|__ EffectScript#0
		///						|__ EffectScript#1
		///						|__ EffectScript#2
		if (_gameObject)
		{
			std::vector<std::string> names;
			int level = 0;
			_gameObject->PrintNames(names, level);
			Point2D pos = ToClientPos(location);
			for (std::string& n : names)
			{
				std::string log{ n };
				log.append("\n");
				pos.Y += offsetZ;
				PrintTextManager::PrintText(log, Colors::Green, pos);
			}
		}
		// 打印叠层信息
		Point2D pos2 = ToClientPos(location);
		for (auto it = _aeStacks.begin(); it != _aeStacks.end(); it++)
		{
			std::string log;
			log.append(it->first).append(" : ").append(std::to_string(it->second)).append("\n");
			pos2.Y -= offsetZ;
			PrintTextManager::PrintText(log, Colors::Red, pos2);
			}
#endif // DEBUG
	}
	else
	{
		// BeginRender
		if (!_ownerIsDead)
		{
			location = MarkLocation();
		}
		// 替身的定位偏移
		std::map<std::string, CoordStruct> standMarks{};
		std::map<int, CoordStruct> standGroupMarks{};
		std::map<int, CoordStruct> standGroupFirstMarks{};
		// 动画的定位偏移
		std::map<std::string, CoordStruct> animMarks{};
		std::map<int, CoordStruct> animGroupMarks{};
		std::map<int, CoordStruct> animGroupFirstMarks{};

		// 火车的位置索引
		int markIndex = 0;
		ForeachChild([&](Component* c) {
			if (auto ae = dynamic_cast<AttachEffectScript*>(c)) {
				if (ae->IsAlive())
				{
					AttachEffectData aeData = ae->AEData;
					// TODO 调整替身的位置
					// if (aeData.Stand.Enable)
					// {
					// }
					// 调整动画的位置
					if (aeData.Animation.Enable && aeData.Animation.IdleAnim.Enable)
					{
						OffsetData offsetData = aeData.Animation.IdleAnim.Offset;
						CoordStruct animOffset = this->StackOffset(aeData, offsetData, animMarks, animGroupMarks, animGroupFirstMarks);
						ae->UpdateAnimOffset(animOffset);
					}
				}
				ae->OnGScreenRender(location);
			}
			});
	}
}

void AttachEffect::OnUpdate()
{
	// 添加Section上记录的AE
	if (!_ownerIsDead)
	{
		_location = pObject->GetCoords();
		// 检查电力
		if (!IsBullet())
		{
			PowerOff = pTechno->Owner->HasLowPower();
			if (!PowerOff && IsBuilding())
			{
				// 关闭当前建筑电源
				PowerOff = !dynamic_cast<BuildingClass*>(pTechno)->HasPower;
			}
		}

		// 添加section自带AE
		Attach(GetTypeData());
		// 检查乘客并附加乘客带来的AE
		if (pTechno)
		{
			// 赋予乘客带来的AE，由于乘客不会在非OpenTopped的载具内执行update事件，因此由乘客向载具赋予AE的任务也由载具执行
			AttachUploadAE();
		}
		// 赋予自带的多组AE
		AttachGroupAE();

		this->_attachOnceFlag = true;
	}
}

void AttachEffect::OnUpdateEnd()
{
	// 移除失效的AE，附加Next的AE
	CheckDurationAndDisable();
}

void AttachEffect::OnWarpUpdate()
{
	// 移除失效的AE，附加Next的AE
	CheckDurationAndDisable();
}

void AttachEffect::OnPut(CoordStruct* pCoord, DirType dirType)
{
	_location = *pCoord;
	// TODO InitEffectFlag
}

void AttachEffect::OnRemove()
{
	// 从地图移除时关闭AE
	_location = pObject->GetCoords();
	CoordStruct location = pObject->GetCoords();
	_location = location;
	ClearLocationMarks();
	ForeachChild([&location](Component* c) {
		if (auto ae = dynamic_cast<AttachEffectScript*>(c))
		{
			if (ae->AEData.DiscardOnEntry)
			{
				ae->End(location);
			}
		}
		});
}

void AttachEffect::CanFire(AbstractClass* pTarget, WeaponTypeClass* pWeapon, bool& ceaseFire)
{
	// TODO 目标免疫超时空和磁电，不能攻击
}

void AttachEffect::OnFire(AbstractClass* pTarget, int weaponIdx)
{
	WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
	WeaponTypeClass* pWeaponType = nullptr;
	if (pWeapon && (pWeaponType = pWeapon->WeaponType) != nullptr)
	{
		FeedbackAttach(pWeaponType);
	}
}

void AttachEffect::OnReceiveDamageDestroy()
{
	_ownerIsDead = true;
};

void AttachEffect::OnDetonate(CoordStruct* pCoords, bool& skip)
{
	_ownerIsDead = true;
	_location = *pCoords;
}
