// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

#include "Perception/AIPerceptionComponent.h"
#include "TowerDefencePawns/Defenders/Warrior/AI/WarriorAIController.h"
#include "TowerDefencePawns/Weapon.h"

AWarrior::AWarrior()
{
    PawnDisplayName = TEXT("Warrior");
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    Sword = CreateDefaultSubobject<AWeapon>(TEXT("Sword"));
    CurrentTeam = EAITeam::Defender;
}

void AWarrior::StartAttack()
{
    bCanAttack = false;
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
        Sword->AttachToSkeleton(SkeletalMesh);
    }
    SkeletalMesh->SetVisibility(bActive);
    SkeletalMesh->SetComponentTickEnabled(bActive);
    SkeletalMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    SkeletalMesh->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    Sword->GetMesh()->SetVisibility(bActive);
    Sword->GetMesh()->SetComponentTickEnabled(bActive && Sword->DoesMeshTick());
    bCanAttack = bActive;
}

void AWarrior::DoUpdatePerceptionOnTeamChange()
{
    if (AWarriorAIController* W_AIC = GetController<AWarriorAIController>())
    {
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*W_AIC->GetAIPerceptionComponent());
        W_AIC->GetAIPerceptionComponent()->ForgetAll();
        W_AIC->GetVisiblePawns().Empty();
        CurrentAttackTarget = nullptr;
    }
}
