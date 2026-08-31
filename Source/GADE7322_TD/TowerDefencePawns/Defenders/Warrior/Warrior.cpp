// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

AWarrior::AWarrior()
{
    PawnDisplayName = TEXT("Warrior");
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
}

void AWarrior::BeginPlay() { Super::BeginPlay(); }

void AWarrior::StartAttack()
{
    bCanAttack = false;
    // I think montage is right?
    SkeletalMesh->GetAnimInstance()->Montage_Play(bIsFemale ? FemaleAttackMontage : MaleAttackMontage, 1.0f);
}

void AWarrior::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void AWarrior::OnDeath(TFunction<void()>&& Func)
{
    DestroyDelegate = MoveTemp(Func);
    SkeletalMesh->GetAnimInstance()->Montage_Play(bIsFemale ? FemaleDeathMontage : MaleDeathMontage, 1.0f);
}

void AWarrior::DoOnSetActive(bool bActive)
{
    if (bActive)
    {
        bIsFemale = FMath::RandBool();
        SkeletalMesh->SetSkeletalMesh(bIsFemale ? FemaleMesh : MaleMesh);
        SkeletalMesh->SetAnimInstanceClass(bIsFemale ? FemaleAnimationBlueprint : MaleAnimationBlueprint);
    }
    SkeletalMesh->SetVisibility(bActive);
    SkeletalMesh->SetComponentTickEnabled(bActive);
    SkeletalMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    SkeletalMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    bCanAttack = bActive;
}
