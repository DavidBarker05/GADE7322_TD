// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Attackers/FlyingEnemy/FlyingEnemy.h"

#include "Animation/AnimInstance.h"
#include "Components/ChildActorComponent.h"
#include "HitFlashComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TowerDefencePawns/Attackers/Skeleton/AI/SkeletonAIController.h"
#include "Weapon.h"

AFlyingEnemy::AFlyingEnemy()
{
    PawnDisplayName = TEXT("Flying Eye");
    OccupiedRadius = 40.0f;
    CurrentTeam = EAITeam::FlyingAttacker;
}

void AFlyingEnemy::BeginPlay()
{
    Super::BeginPlay();
    HitFlashComponent->BindMaterials();
}

void AFlyingEnemy::StartAttack()
{
    bCanAttack = false;
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(AttackMontage, 1.0f);
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &AFlyingEnemy::OnAttackMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void AFlyingEnemy::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void AFlyingEnemy::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted BlendingOutDelegate;
    BlendingOutDelegate.BindUObject(this, &AFlyingEnemy::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, DeathMontage);
}

void AFlyingEnemy::DoOnSetActive(bool bActive)
{
    if (bActive)
    {
        CurrentAttackTarget = nullptr;
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            AnimInstance->StopAllMontages(0.0f);
            AnimInstance->InitializeAnimation();
        }
    }
    GetMesh()->SetVisibility(bActive);
    GetMesh()->SetComponentTickEnabled(bActive);
    GetMesh()->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    if (bActive)
    {
        GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
    bCanAttack = bActive;
}

void AFlyingEnemy::DoUpdatePerceptionOnTeamChange()
{
    if (ASkeletonAIController* S_AIC = GetController<ASkeletonAIController>())
    {
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*S_AIC->GetAIPerceptionComponent());
        S_AIC->GetAIPerceptionComponent()->ForgetAll();
        S_AIC->GetVisiblePawns().Empty();
        CurrentAttackTarget = nullptr;
    }
}

void AFlyingEnemy::OnAttackMontageEnded(UAnimMontage*, bool) { EndAttack(); }

void AFlyingEnemy::OnDeathMontageEnded(UAnimMontage*, bool) { OnDeathComplete(); }
