#include "TowerDefencePawns/Tower/PlayerTower.h"

#include "DamageComponent.h"

APlayerTower::APlayerTower()
{
    PawnDisplayName = TEXT("Main Tower");
    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Mesh"));
    TowerMesh->SetupAttachment(RootComponent);
}

void APlayerTower::StartAttack()
{
    for (int32 i = 0; i < 3; ++i)
    {
        if (IsValid(AttackTargets[i]) && CanAttackTarget[i])
        {
            CanAttackTarget[i] = false;
            Attack(AttackTargets[i]);
            // TODO: Fire laser
            GetWorldTimerManager().SetTimer(
                TimerHandles[i], [this, i]() -> void { CanAttackTarget[i] = true; }, AttackCooldown, false);
        }
    }
}
