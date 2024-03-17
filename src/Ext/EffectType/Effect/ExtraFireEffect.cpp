#include "ExtraFireEffect.h"

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>
#include <Ext/Helper/Weapon.h>

#include <Extension/BulletExt.h>
#include <Extension/TechnoExt.h>

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/ObjectType/AttachFire.h>


bool ExtraFireEffect::CheckROF(WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData* weaponData)
{
	bool canFire = false;
	std::string weaponId = pWeapon->ID;
	// 进行ROF检查
	canFire = !weaponData->CheckROF;
	if (!canFire)
	{
		auto it = _rof.find(weaponId);
		if (it != _rof.end())
		{
			canFire = it->second.Expired();
		}
		else
		{
			canFire = true;
		}
	}
	return canFire;
}

void ExtraFireEffect::ResetROF(WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData* weaponData, double rofMultip)
{
	std::string weaponId = pWeapon->ID;
	int rof = (int)(pWeapon->ROF * rofMultip);
	auto it = _rof.find(weaponId);
	if (it != _rof.end())
	{
		it->second.Start(rof);
	}
	else
	{
		CDTimerClass timer{ rof };
		_rof[weaponId] = timer;
	}
}

void ExtraFireEffect::OnFire(AbstractClass* pTarget, int weaponIdx)
{
	ExtraFireEntity data = Data->Data;
	ExtraFireFLHEntity flhData = Data->FLHData;
	bool isElite = pTechno->Veterancy.IsElite();
	if (isElite)
	{
		data = Data->EliteData;
		flhData = Data->EliteFLHData;
	}
	if (data.Enable)
	{
		TechnoClass* pShooter = WhoIsShooter(pTechno);
		if (!flhData.Enable)
		{
			// 取Art上的设置
			const char* section = pShooter->GetTechnoType()->ID;
			std::string image = INI::GetSection(INI::Rules, section)->Get("Image", std::string{ section });
			ExtraFireFLHData* pArtData = INI::GetConfig<ExtraFireFLHData>(INI::Art, image.c_str())->Data;
			if (isElite)
			{
				ExtraFireFLHEntity art = pArtData->EliteData;
				if (art.Enable)
				{
					flhData = art;
				}
			}
			else
			{
				ExtraFireFLHEntity art = pArtData->Data;
				if (art.Enable)
				{
					flhData = art;
				}
			}
		}
		// 准备发射
		std::vector<std::string> weaponTypes{};
		CoordStruct fireFLH = CoordStruct::Empty;
		bool isOnBody = false;
		bool isOnTarget = false;
		if (pTechno->GetTechnoType()->WeaponCount > 0)
		{
			// 盖特
			if (!data.WeaponX.empty())
			{
				auto it = data.WeaponX.find(weaponIdx);
				if (it != data.WeaponX.end())
				{
					weaponTypes = it->second;
				}
			}
			if (flhData.Enable && !flhData.WeaponXFLH.empty())
			{
				auto it = flhData.WeaponXFLH.find(weaponIdx);
				if (it != flhData.WeaponXFLH.end())
				{
					fireFLH = it->second;
				}

				auto it2 = std::find(flhData.OnBodyIndexs.begin(), flhData.OnBodyIndexs.end(), weaponIdx);
				isOnBody = it2 != flhData.OnBodyIndexs.end();

				auto it3 = std::find(flhData.OnTargetIndexs.begin(), flhData.OnTargetIndexs.end(), weaponIdx);
				isOnTarget = it3 != flhData.OnTargetIndexs.end();
			}
		}
		else if (weaponIdx == 0)
		{
			// 主武器
			weaponTypes = data.Primary;
			if (flhData.Enable)
			{
				fireFLH = flhData.PrimaryFLH;
				isOnBody = flhData.PrimaryOnBody;
				isOnTarget = flhData.PrimaryOnTarget;
			}
		}
		else if (weaponIdx == 1)
		{
			// 副武器
			weaponTypes = data.Secondary;
			if (flhData.Enable)
			{
				fireFLH = flhData.SecondaryFLH;
				isOnBody = flhData.SecondaryOnBody;
				isOnTarget = flhData.SecondaryOnTarget;
			}
		}

		// 开始发射武器
		if (!weaponTypes.empty())
		{
			double rofMultip = GetROFMulti(pTechno);
			rofMultip *= AE->AEManager->CountAttachStatusMultiplier().ROFMultiplier;
			if (AttachFire* attachFire = FindOrAttachScript<TechnoExt, AttachFire>(pShooter))
			{
				for (std::string weaponId : weaponTypes)
				{
					// 发射前检查
					if (WeaponTypeClass* pWeapon = WeaponTypeClass::Find(weaponId.c_str()))
					{
						WeaponTypeExt::TypeData* weaponData = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon);

						bool canFire = !weaponData->CheckROF;
						if (!canFire)
						{
							canFire = CheckROF(pWeapon, weaponData);
							if (canFire)
							{
								ResetROF(pWeapon, weaponData, rofMultip);
							}
						}
						if (canFire && (!weaponData->OnlyFireInTransport || pTechno->Transporter))
						{
							if (weaponData->UseAlternateFLH && pTechno->Transporter)
							{
								// 获取开或者位于载具的序号，再获取载具的开火坐标，先进后出，序号随着乘客数量增长
								int index = pTechno->Transporter->Passengers.IndexOf(dynamic_cast<FootClass*>(pTechno));
								// 有效序号1 - 5, 对应FLH 0-4
								if (index > 0 && index < 6)
								{
									fireFLH = pTechno->Transporter->GetTechnoType()->AlternativeFLH[index - 1];
								}
							}
							if (weaponData->Feedback)
							{
								// 调转枪口
								attachFire->FireCustomWeapon(nullptr, pTechno, pTechno->Owner, pWeapon, *weaponData, fireFLH, isOnBody, isOnTarget);
							}
							else
							{
								attachFire->FireCustomWeapon(pTechno, pTarget, pTechno->Owner, pWeapon, *weaponData, fireFLH, isOnBody, isOnTarget);
							}
						}
					}

				}
			}

		}

	}
}

