// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

ASkeletonPawn::ASkeletonPawn()
{
    PawnDisplayName = TEXT("Skeleton");
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
}

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
    if (bActive) SkeletalMesh->SetSkeletalMesh(PossibleMeshes[FMath::RandRange(0, PossibleMeshes.Num() - 1)]);
    SkeletalMesh->SetVisibility(bActive);
    SkeletalMesh->SetComponentTickEnabled(bActive);
    SkeletalMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    SkeletalMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    bCanAttack = bActive;
}
