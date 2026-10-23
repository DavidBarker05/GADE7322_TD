#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"
#include "TowerDefencePawns/Defenders/Warrior/AI/WarriorAIController.h"
#include "TowerDefencePawns/Defenders/Warrior/WarriorShield.h"
#include "TowerDefencePawns/Weapon.h"

AWarrior::AWarrior()
{
    PawnDisplayName = TEXT("Warrior");
    OccupiedRadius = 40.0f;
    OneHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("One Hand Weapon"));
    OneHandWeapon->SetupAttachment(RootComponent);
    WarriorShield = CreateDefaultSubobject<UChildActorComponent>(TEXT("Warrior Shield"));
    WarriorShield->SetupAttachment(RootComponent);
    TwoHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Two Hand Weapon"));
    TwoHandWeapon->SetupAttachment(RootComponent);
    CurrentTeam = EAITeam::MeleeDefender;
    PrimaryRadiusColour = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void AWarrior::StartAttack()
{
    bCanAttack = false;
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage* AttackMontage = GetCurrentWarrior().GetAttackMontage();
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
    UAnimMontage* DeathMontage = GetCurrentWarrior().GetDeathMontage();
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted EndDelegate;
    EndDelegate.BindUObject(this, &AWarrior::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(EndDelegate, DeathMontage);
}

void AWarrior::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    CurrentAttackTarget = nullptr;
    if (bActive) GetUpgradeComponent()->SetLevel(0); // Reset to base level
    else
    {
        if (AWeapon* OneHandSword = GetOneHandSword(); IsValid(OneHandSword))
        {
            OneHandSword->AttachToComponent(OneHandWeapon, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            OneHandSword->GetMesh()->SetVisibility(false);
            OneHandSword->GetMesh()->SetComponentTickEnabled(false);
        }
        if (AWarriorShield* Shield = GetShield(); IsValid(Shield))
        {
            Shield->AttachToComponent(WarriorShield, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            Shield->GetMesh()->SetVisibility(false);
            Shield->GetMesh()->SetComponentTickEnabled(false);
        }
        if (AWeapon* TwoHandSword = GetTwoHandSword(); IsValid(TwoHandSword))
        {
            TwoHandSword->AttachToComponent(TwoHandWeapon, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            TwoHandSword->GetMesh()->SetVisibility(false);
            TwoHandSword->GetMesh()->SetComponentTickEnabled(false);
        }
        HitFlashComponent->UnbindMaterials();
    }
    GetMesh()->SetVisibility(bActive);
    GetMesh()->SetComponentTickEnabled(bActive);
    GetMesh()->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    if (bActive)
    {
        GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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
void AWarrior::DoOnLevelChanged(int32 Level)
{
    const FDefenderLevel CurrentDefenderLevel = GetUpgradeComponent()->GetCurrent();
    const FWarriorLevel CurrentWarriorLevel = GetWarriorCurrentLevel();
    AWeapon* Sword = GetWeapon();
    if (Level == 0 || Level == 1)
    {
        bIsFemale = FMath::RandBool();
        if (Level == 1)
        {
            if (AWarriorShield* Shield = GetShield(); IsValid(Shield))
            {
                Shield->AttachToSkeleton(GetMesh());
                Shield->GetMesh()->SetVisibility(true);
                Shield->GetMesh()->SetComponentTickEnabled(Shield->DoesMeshTick());
            }
        }
    }
    FWarriorSkeletalMesh CurrentWarrior = GetCurrentWarrior();
    GetMesh()->SetSkeletalMesh(CurrentWarrior.GetSkeletalMesh());
    GetMesh()->SetAnimInstanceClass(CurrentWarrior.GetAnimationBlueprint());
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentWarrior.GetHalfHeight());
    FVector NewMeshLoc = GetMesh()->GetRelativeLocation();
    NewMeshLoc.Z = -CurrentWarrior.GetHalfHeight();
    GetMesh()->SetRelativeLocation(NewMeshLoc);
    FVector NewBoxExtent = GetBoxCollider()->GetUnscaledBoxExtent();
    NewBoxExtent.Z = CurrentWarrior.GetHalfHeight();
    GetBoxCollider()->SetBoxExtent(NewBoxExtent);
    DamageComponent->SetDamage(CurrentDefenderLevel.GetDamage());
    HealthComponent->SetMaxHealth(CurrentDefenderLevel.GetHealth());
    AttackRadius = CurrentWarriorLevel.GetAttackRadius();
    AttackCooldown = CurrentWarriorLevel.GetAttackCooldown();
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->StopAllMontages(0.0f);
        AnimInstance->InitializeAnimation();
    }
    HitFlashComponent->BindMaterials();
    if (IsValid(Sword))
    {
        Sword->AttachToSkeleton(GetMesh());
        Sword->GetMesh()->SetVisibility(true);
        Sword->GetMesh()->SetComponentTickEnabled(Sword->DoesMeshTick());
    }
}

FWarriorSkeletalMesh AWarrior::GetCurrentWarrior() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    return CurrentLevel == 0 || CurrentLevel == 1 ? (bIsFemale ? FemaleWarrior : MaleWarrior) : FinalWarrior;
}

FWarriorLevel AWarrior::GetWarriorCurrentLevel() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    if (CurrentLevel == 0) return WarriorBaseLevel;
    if (CurrentLevel == 1) return WarriorFirstUpgrade;
    return WarriorSecondUpgrade;
}

const AWeapon* AWarrior::GetWeapon() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    const UChildActorComponent* Weapon = CurrentLevel == 0 || CurrentLevel == 1 ? OneHandWeapon : TwoHandWeapon;
    return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr);
}

AWeapon* AWarrior::GetWeapon()
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    const UChildActorComponent* Weapon = CurrentLevel == 0 || CurrentLevel == 1 ? OneHandWeapon : TwoHandWeapon;
    return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr);
}

AWeapon* AWarrior::Blueprint_GetWeapon() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    const UChildActorComponent* Weapon = CurrentLevel == 0 || CurrentLevel == 1 ? OneHandWeapon : TwoHandWeapon;
    return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr);
}

AWeapon* AWarrior::GetOneHandSword() const
{
    return Cast<AWeapon>(OneHandWeapon ? OneHandWeapon->GetChildActor() : nullptr);
}

AWarriorShield* AWarrior::GetShield() const
{
    return Cast<AWarriorShield>(WarriorShield ? WarriorShield->GetChildActor() : nullptr);
}

AWeapon* AWarrior::GetTwoHandSword() const
{
    return Cast<AWeapon>(TwoHandWeapon ? TwoHandWeapon->GetChildActor() : nullptr);
}

void AWarrior::OnAttackMontageEnded(UAnimMontage*, bool) { EndAttack(); }

void AWarrior::OnDeathMontageEnded(UAnimMontage*, bool) { OnDeathComplete(); }
