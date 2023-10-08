#include <exception>
#include <Windows.h>

#include <TechnoTypeClass.h>
#include <TechnoClass.h>
#include <AircraftTypeClass.h>
#include <AircraftClass.h>
#include <SpawnManagerClass.h>
#include <Kamikaze.h>

#include <Extension.h>
#include <Helpers/Macro.h>
#include <Ext/Helper/Status.h>
#include <Ext/TechnoStatus.h>
#include <Ext/AttachFire.h>
#include <Extension/TechnoExt.h>

static bool TryFindNewIdInSpwanType(std::string typeId, int index, std::string& newId)
{
	if (typeId.find(",") != std::string::npos)
	{
		std::vector<std::string> values = {};
		char* context = nullptr;
		for (auto cur = strtok_s(const_cast<char*>(typeId.c_str()), ",", &context); cur && *cur; cur = strtok_s(nullptr, ",", &context)) {
			std::string tmp{ cur };
			values.push_back(tmp);
		}
		int size = values.size();
		if (index < size)
		{
			newId = trim(values[index]);
			return true;
		}
	}
	return false;
}


DEFINE_HOOK(0x6B6D4D, SpawnManagerClass_CreateSpawnNode_CreateAircraft, 0x6)
{
	GET(SpawnManagerClass*, pManager, ESI);
	GET_STACK(int, i, 0x20 + 0x4);

	std::string newId{ "" };
	TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pManager->Owner);
	// 如果没有设置SpwanData，就从ini名字中找
	if (!status || !status->TryGetSpawnType(i, newId))
	{
		GET(TechnoTypeClass*, pType, ECX);
		std::string typeId{ pType->ID };
		TryFindNewIdInSpwanType(typeId, i, newId);
	}
	if (IsNotNone(newId))
	{
		AircraftTypeClass* pNewType = AircraftTypeClass::Find(newId.c_str());
		if (pNewType)
		{
			pManager->SpawnType = pNewType;
			R->ECX(reinterpret_cast<unsigned int>(pNewType));
		}
	}

	return 0;
}

DEFINE_HOOK(0x6B78E4, SpawnManagerClass_Update_CreateAircraft, 0x6)
{
	GET(SpawnManagerClass*, pManager, ESI);
	GET(int, i, EBX);

	std::string newId{ "" };
	TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pManager->Owner);
	// 如果没有设置SpwanData，就从ini名字中找
	if (!status || !status->TryGetSpawnType(i, newId))
	{
		GET(TechnoTypeClass*, pType, ECX);
		std::string typeId{ pType->ID };
		TryFindNewIdInSpwanType(typeId, i, newId);
	}
	if (IsNotNone(newId))
	{
		AircraftTypeClass* pNewType = AircraftTypeClass::Find(newId.c_str());
		if (pNewType)
		{
			pManager->SpawnType = pNewType;
			R->ECX(reinterpret_cast<unsigned int>(pNewType));
		}
	}

	return 0;
}

DEFINE_HOOK(0x6B743E, SpawnManagerClass_Update_PutSpawns, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	GET(SpawnManagerClass*, pManager, ESI);
	GET(int, weaponIdx, EBP);

	bool customFLH = false;
	if (weaponIdx > 0)
	{
		// wwsb 主武器没有Spawner，所以用副武器，继续检查其他武器
		// 检查副武器有没有Spawner
		WeaponStruct* pWeapon = pTechno->GetWeapon(weaponIdx);
		bool spawner = false;
		if (pWeapon && pWeapon->WeaponType)
		{
			spawner = pWeapon->WeaponType->Spawner;
		}
		if (!spawner)
		{
			// 副武器上也没有Spawner，继续找盖特武器其他的武器
			int weaponCount = pTechno->GetTechnoType()->WeaponCount;
			if (weaponCount > 2)
			{
				for (int i = 2; i < weaponCount; i++)
				{
					pWeapon = pTechno->GetWeapon(i);
					WeaponTypeClass* pWeaponType = nullptr;
					if (pWeapon && (pWeaponType = pWeapon->WeaponType) != nullptr && pWeaponType->Spawner)
					{
						// 找到一个子机发射器
						spawner = true;
						weaponIdx = i;
						// Logger.Log($"{Game.CurrentFrame} [{pTechno->Type->Base.Base.ID}]{pTechno} Weapon {i} / {weaponCount} = [{pWeapon->WeaponType->Base.ID}]");
						break;
					}
				}
			}
		}
		if (spawner)
		{
			// 找到另外的子机发射器，设置Index
			R->EBP(static_cast<unsigned int>(weaponIdx));
		}
		else if (AttachFire<TechnoClass, TechnoExt>* pFire = GetComponent<TechnoExt, AttachFire<TechnoClass, TechnoExt>>(pTechno))
		{
			if (!pFire->SpawnerBurstFLH.empty())
			{
				int index = 0;
				int count = pManager->SpawnCount;
				if (count > 1)
				{
					index = count - pManager->CountDockedSpawns() - 1;
					// Logger.Log($"{Game.CurrentFrame} [{pTechno->Type->Base.Base.ID}]{pTechno} 发射子机 {index} - {pManager->DrawState()}/{count}");
				}
				auto it = pFire->SpawnerBurstFLH.find(index);
				customFLH = it != pFire->SpawnerBurstFLH.end();
				if (customFLH)
				{
					// 副武器和盖特武器上都没有子机发射器，检查子机是否由ExtraFire或者AutoWeapon发射
					GET(CoordStruct*, eax, EAX);
					*eax = pFire->SpawnerBurstFLH[index];
				}
			}
		}
	}
	// 重设子机发射延迟
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		// 重设子机发射延迟
		if (status->GetSpawnData()->SpawnDelay > -1)
		{
			pManager->SpawnTimer.Start(status->GetSpawnData()->SpawnDelay);
		}
	}
	if (customFLH)
	{
		return 0x6B7498;
	}
	return 0;
}

DEFINE_HOOK(0x6B796A, SpawnManagerClass_Update_PutSpawns_FireOnce, 0x5)
{
	GET(SpawnManagerClass*, pManager, ESI);
	if (pManager->CountDockedSpawns() == 0)
	{
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pManager->Owner))
		{
			if (status->GetSpawnData()->SpawnFireOnce)
			{
				pManager->Destination = nullptr;
				pManager->SetTarget(nullptr);
			}
		}
	}
	return 0;
}

#pragma region Rocket Homing
// 如果子机管理器输入的预设目标在天上，自动开启跟踪模式
DEFINE_HOOK(0x6B7A32, SpawnManagerClass_Update_Add_Missile_Target, 0x5)
{
	GET(TechnoClass*, pRocket, ECX);
	GET(AbstractClass*, pTarget, EAX);

	if (pTarget && pTarget->IsInAir())
	{
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pRocket))
		{
			status->IsHoming = true;
		}
	}

	return 0;
}

// 将子机管理器的目标输入给导弹本体
DEFINE_HOOK(0x54E42B, KamikazeTrackerClass_Add_Missile_Has_Target, 0x6)
{
	GET_STACK(AircraftClass*, pRocket, 0x1C);
	GET(AbstractClass*, pTarget, ECX);

	pRocket->SetTarget(pTarget);
	return 0;
}

// 如果导弹本体有目标，则修改导弹控制器的目标为导弹本体的目标
DEFINE_HOOK(0x54E478, KamikazeTrackerClass_Add, 0x5)
{
	GET(Kamikaze::KamikazeControl*, pKamikazeControl, EBX);
	AircraftClass* pRocket = pKamikazeControl->Item;
	AbstractClass* pTarget = pRocket->Target;
	if (pTarget)
	{
		pKamikazeControl->Cell = pTarget;
	}
	return 0;
}

// 导弹追击的目标死亡，需要重设目标
DEFINE_HOOK(0x54E661, KamikazeTrackerClass_Cannot_Detach2, 0x6)
{
	GET(Kamikaze::KamikazeControl*, pKamikazeControl, EAX);
	AircraftClass* pRocket = pKamikazeControl->Item;
	if (IsDeadOrInvisible(static_cast<TechnoClass*>(pRocket)))
	{
		// 导弹作为目标时，死亡的是导弹，也是目标
		return 0;
	}
	AbstractClass* pTarget = pKamikazeControl->Cell;
	TechnoClass* pTargetTechno = nullptr;
	if (!CastToTechno(pTarget, pTargetTechno) || IsDeadOrInvisible(pTargetTechno))
	{
		// 目标是个死人，替换目标
		CoordStruct lastLocation = pRocket->GetCoords();
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(static_cast<TechnoClass*>(pRocket)))
		{
			status->IsHoming = false;
			lastLocation = status->HomingTargetLocation;
		}
		// 获取最后目标所在的格子，然后重设导弹目标
		if (CellClass* pCell = MapClass::Instance->TryGetCellAt(lastLocation))
		{
			pKamikazeControl->Cell = pCell;
		}
	}
	return 0;
}

DEFINE_HOOK(0x6622C0, RocketLocomotionClass_Process, 0x6)
{
	GET(FootClass*, pFoot, ESI);
	RocketLocomotionClass* pLoco = static_cast<RocketLocomotionClass*>(pFoot->Locomotor.get());
	// If missile try to AA, it will block on ground. step is 0, can't move.
	if (pLoco->MissionState == 0)
	{
		pLoco->MissionState = 1;
	}
	return 0;
}

DEFINE_HOOK(0x662CAC, RocketLocomotionClass_Process_Step5_To_Lazy_4, 0x6)
{
	GET(RocketLocomotionClass*, pLoco, ESI);
	pLoco -= 1; // ESI - 4 才是真正的指针地址
	TechnoClass* pRocket = pLoco->LinkedTo;
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pRocket))
	{
		if (status->IsHoming)
		{
			return 0x662A32;
		}
	}
	return 0;
}

DEFINE_HOOK(0x66304F, RocketLocomotionClass_663030, 0x5)
{
	GET(TechnoClass*, pThis, EDX);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ if (auto cc = dynamic_cast<TechnoScript*>(c)) { cc->OnRocketExplosion(); } });
	return 0;
}
#pragma endregion


