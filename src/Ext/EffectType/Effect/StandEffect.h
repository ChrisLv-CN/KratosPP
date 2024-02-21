#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <TechnoClass.h>

#include "../EffectScript.h"
#include "StandData.h"


/// @brief EffectScript
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class StandEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Stand);

	void UpdateLocation(LocationMark LocationMark);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args);

	virtual void OnStart() override;

	virtual void End(CoordStruct location) override;

	virtual void OnPause() override;
	virtual void OnRecover() override;

	virtual bool IsAlive() override;

	virtual void OnGScreenRender(CoordStruct location) override;

	virtual void OnPut(CoordStruct* pCoord, DirType faceDir) override;
	virtual void OnUpdate() override;
	virtual void OnWarpUpdate() override;
	virtual void OnTemporalEliminate(TemporalClass* pTemporal) override;
	virtual void OnRemove() override;
	virtual void OnReceiveDamageDestroy() override;
	virtual void OnGuardCommand() override;
	virtual void OnStopCommand() override;

	virtual void OnRocketExplosion() override;

	TechnoClass* pStand = nullptr;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->pStand)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		EventSystems::Logic.AddHandler(Events::TechnoDeleteEvent, this, &StandEffect::OnTechnoDelete);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<StandEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
    bool masterIsRocket = false;
    bool masterIsSpawned = false;
	bool standIsBuilding = false;
	bool onStopCommand = false;
    bool notBeHuman = false;
    bool onReceiveDamageDestroy = false;

    LocationMark _lastLocationMark;
    bool _isMoving = false;
	CDTimerClass _walkRateTimer;

	void CreateAndPutStand();
	void SetLocation(CoordStruct location);
	void SetFacing(DirStruct dir, bool forceSetTurret);
	void ForceFacing(DirStruct dir);

	void ExplodesOrDisappear(bool peaceful);

	void UpdateStateBullet();
	void UpdateStateTechno(bool masterIsDead);

	void RemoveStandIllegalTarget();
};
