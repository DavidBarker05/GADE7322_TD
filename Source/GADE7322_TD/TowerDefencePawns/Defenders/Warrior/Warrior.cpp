// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

#include "Animation/AnimInstance.h"
#include "Components/ChildActorComponent.h"
#include "HitFlashComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TowerDefencePawns/Defenders/Warrior/AI/WarriorAIController.h"
#include "TowerDefencePawns/Weapon.h"

AWarrior::AWarrior()
{
    PawnDisplayName = TEXT("Warrior");
    OccupiedRadius = 40.0f;
    Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
    Weapon->SetupAttachment(RootComponent);
    CurrentTeam = EAITeam::Defender;
}

const AWeapon* AWarrior::GetWeapon() const { return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr); }

AWeapon* AWarrior::GetWeapon() { return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr); }

void AWarrior::StartAttack()
{
    bCanAttack = false;
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage* AttackMontage = bIsFemale ? FemaleAttackMontage : MaleAttackMontage;
    AnimInstance->Montage_Play(AttackMontage, 1.0f);
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &AWarrior::OnAttackMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void AWarrior::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void AWarrior::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage* DeathMontage = bIsFemale ? FemaleDeathMontage : MaleDeathMontage;
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted EndDelegate;
    EndDelegate.BindUObject(this, &AWarrior::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(EndDelegate, DeathMontage);
}

void AWarrior::DoOnSetActive(bool bActive)
{
    if (bActive) CurrentAttackTarget = nullptr;
    AWeapon* Sword = GetWeapon();
    if (bActive)
    {
        bIsFemale = FMath::RandBool();
        GetMesh()->SetSkeletalMesh(bIsFemale ? FemaleMesh : MaleMesh);
        GetMesh()->SetAnimInstanceClass(bIsFemale ? FemaleAnimationBlueprint : MaleAnimationBlueprint);
        if (Sword) Sword->AttachToSkeleton(GetMesh());
        HitFlashComponent->BindMaterials();
    }
    else
    {
        if (Sword) Sword->AttachToComponent(Weapon, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        HitFlashComponent->UnbindMaterials();
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
    if (Sword)
    {
        Sword->GetMesh()->SetVisibility(bActive);
        Sword->GetMesh()->SetComponentTickEnabled(bActive && Sword->DoesMeshTick());
    }
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

void AWarrior::OnAttackMontageEnded(UAnimMontage*, bool) { EndAttack(); }

void AWarrior::OnDeathMontageEnded(UAnimMontage*, bool) { OnDeathComplete(); }
