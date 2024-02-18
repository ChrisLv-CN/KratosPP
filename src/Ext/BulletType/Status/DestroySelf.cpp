#include "../BulletStatus.h"

void BulletStatus::OnUpdate_DestroySelf()
{
	if (DestroySelfState->AmIDead())
	{
		// 啊我死了
		bool harmless = DestroySelfState->Data.Peaceful;
		// Logger.Log("抛射体[{0}]{1}自毁倒计时结束，自毁开始{2}", OwnerObject.Ref.Type.Ref.Base.Base.ID, OwnerObject, bulletDamageStatus);
		TakeDamage(0, true, harmless, false);
	}
}
