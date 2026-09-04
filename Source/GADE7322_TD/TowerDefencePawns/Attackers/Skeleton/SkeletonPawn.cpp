// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"

#include "Components/ChildActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TowerDefencePawns/Attackers/Skeleton/AI/SkeletonAIController.h"
#include "Weapon.h"

ASkeletonPawn::ASkeletonPawn()
{
    PawnDisplayName = TEXT("Skeleton");
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
    Weapon->SetupAttachment(RootComponent);
    CurrentTeam = EAITeam::Attacker;
}

const AWeapon* ASkeletonPawn::GetWeapon() const { return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr); }

AWeapon* ASkeletonPawn::GetWeapon() { return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr); }

void ASkeletonPawn::BeginPlay() { Super::BeginPlay(); }

void ASkeletonPawn::StartAttack()
{
    bCanAttack = false;
    SkeletalMesh->GetAnimInstance()->Montage_Play(AttackMontage, 1.0f);
}

void ASkeletonPawn::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void ASkeletonPawn::OnDeath(TFunction<void()>&& Func)
{
    DestroyDelegate = MoveTemp(Func);
    SkeletalMesh->GetAnimInstance()->Montage_Play(DeathMontage, 1.0f);
}

void ASkeletonPawn::DoOnSetActive(bool bActive)
{
    AWeapon* Sword = GetWeapon();
    if (bActive && Sword) Sword->AttachToSkeleton(SkeletalMesh);
    SkeletalMesh->SetVisibility(bActive);
    SkeletalMesh->SetComponentTickEnabled(bActive);
    SkeletalMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    SkeletalMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    if (Sword)
    {
        Sword->GetMesh()->SetVisibility(bActive);
        Sword->GetMesh()->SetComponentTickEnabled(bActive && Sword->DoesMeshTick());
    }
    bCanAttack = bActive;
}

void ASkeletonPawn::DoUpdatePerceptionOnTeamChange()
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
