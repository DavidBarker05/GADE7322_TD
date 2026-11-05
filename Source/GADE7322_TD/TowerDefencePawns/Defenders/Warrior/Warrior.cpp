#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/DecalComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"
#include "TowerDefencePawns/Defenders/Warrior/AI/WarriorAIController.h"
#include "TowerDefencePawns/Defenders/Warrior/WarriorShield.h"
#include "TowerDefencePawns/Defenders/Warrior/WarriorWeapon.h"

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

void AWarrior::BeginPlay()
{
    Super::BeginPlay();
    if (AWarriorWeapon* OneHandSword = GetOneHandSword(); IsValid(OneHandSword))
    {
        OneHandSword->GetHitFlashComponent()->BindMaterials();
        OneHandSword->GetOutlineComponent()->BindMaterials();
    }
    if (AWarriorShield* Shield = GetShield(); IsValid(Shield))
    {
        Shield->GetHitFlashComponent()->BindMaterials();
        Shield->GetOutlineComponent()->BindMaterials();
    }
    if (AWarriorWeapon* TwoHandSword = GetTwoHandSword(); IsValid(TwoHandSword))
    {
        TwoHandSword->GetHitFlashComponent()->BindMaterials();
        TwoHandSword->GetOutlineComponent()->BindMaterials();
    }
}

void AWarrior::StartAttack()
{
    bCanAttack = false;
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage* AttackMontage =
        (bIsFemale ? GetWarriorCurrentLevel().GetFemaleWarrior() : GetWarriorCurrentLevel().GetMaleWarrior())
            .GetAttackMontage();
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
    UAnimMontage* DeathMontage =
        (bIsFemale ? GetWarriorCurrentLevel().GetFemaleWarrior() : GetWarriorCurrentLevel().GetMaleWarrior())
            .GetDeathMontage();
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted EndDelegate;
    EndDelegate.BindUObject(this, &AWarrior::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(EndDelegate, DeathMontage);
}

void AWarrior::DoFlash()
{
    Super::DoFlash();
    if (AWarriorWeapon* Sword = GetWeapon(); IsValid(Sword)) Sword->GetHitFlashComponent()->DoFlash();
    if (GetUpgradeComponent()->GetCurrentLevel() != 1) return;
    if (AWarriorShield* Shield = GetShield(); IsValid(Shield)) Shield->GetHitFlashComponent()->DoFlash();
}

void AWarrior::ShowOutline()
{
    Super::ShowOutline();
    if (AWarriorWeapon* Sword = GetWeapon(); IsValid(Sword)) Sword->GetOutlineComponent()->EnableOutline();
    if (GetUpgradeComponent()->GetCurrentLevel() != 1) return;
    if (AWarriorShield* Shield = GetShield(); IsValid(Shield)) Shield->GetOutlineComponent()->EnableOutline();
}

void AWarrior::HideOutline()
{
    Super::HideOutline();
    if (AWarriorWeapon* Sword = GetWeapon(); IsValid(Sword)) Sword->GetOutlineComponent()->DisableOutline();
    if (GetUpgradeComponent()->GetCurrentLevel() != 1) return;
    if (AWarriorShield* Shield = GetShield(); IsValid(Shield)) Shield->GetOutlineComponent()->DisableOutline();
}

void AWarrior::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    CurrentAttackTarget = nullptr;
    if (bActive)
    {
        bIsFemale = FMath::RandBool();
        GetUpgradeComponent()->SetLevel(0); // Reset to base level
        ResetHealth();
    }
    else
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            AnimInstance->StopAllMontages(0.0f);
            AnimInstance->InitializeAnimation();
        }
        if (AWarriorWeapon* OneHandSword = GetOneHandSword(); IsValid(OneHandSword))
        {
            OneHandSword->GetMesh()->SetVisibility(false);
            OneHandSword->GetMesh()->SetComponentTickEnabled(false);
            OneHandSword->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AWarriorShield* Shield = GetShield(); IsValid(Shield))
        {
            Shield->GetMesh()->SetVisibility(false);
            Shield->GetMesh()->SetComponentTickEnabled(false);
            Shield->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AWarriorWeapon* TwoHandSword = GetTwoHandSword(); IsValid(TwoHandSword))
        {
            TwoHandSword->GetMesh()->SetVisibility(false);
            TwoHandSword->GetMesh()->SetComponentTickEnabled(false);
            TwoHandSword->GetHitFlashComponent()->ForceStopFlash();
        }
        HitFlashComponent->ForceStopFlash();
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
    // vvv Reset animation, detach held items and unbind materials because the mesh is about to change vvv
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->StopAllMontages(0.0f);
        AnimInstance->InitializeAnimation();
    }
    const bool bMustResumeFlash = HitFlashComponent->PauseFlash();
    const float RemainingFlashDuration = bMustResumeFlash ? HitFlashComponent->GetRemainingFlashDuration() : 0.0f;
    if (AWarriorWeapon* OneHandSword = GetOneHandSword(); IsValid(OneHandSword))
    {
        OneHandSword->AttachToComponent(OneHandWeapon, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        OneHandSword->GetMesh()->SetVisibility(false);
        OneHandSword->GetMesh()->SetComponentTickEnabled(false);
        OneHandSword->GetHitFlashComponent()->ForceStopFlash();
        OneHandSword->GetOutlineComponent()->DisableOutline();
    }
    AWarriorShield* Shield = GetShield();
    if (IsValid(Shield))
    {
        Shield->AttachToComponent(WarriorShield, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        Shield->GetMesh()->SetVisibility(false);
        Shield->GetMesh()->SetComponentTickEnabled(false);
        Shield->GetHitFlashComponent()->ForceStopFlash();
        Shield->GetOutlineComponent()->DisableOutline();
    }
    if (AWarriorWeapon* TwoHandSword = GetTwoHandSword(); IsValid(TwoHandSword))
    {
        TwoHandSword->AttachToComponent(TwoHandWeapon, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        TwoHandSword->GetMesh()->SetVisibility(false);
        TwoHandSword->GetMesh()->SetComponentTickEnabled(false);
        TwoHandSword->GetHitFlashComponent()->ForceStopFlash();
        TwoHandSword->GetOutlineComponent()->DisableOutline();
    }
    HitFlashComponent->UnbindMaterials();
    OutlineComponent->UnbindMaterials();
    // ^^^ Reset animation, detach held items and unbind materials because the mesh is about to change ^^^

    const FWarriorLevel CurrentWarriorLevel = GetWarriorCurrentLevel();
    FWarriorSkeletalMesh CurrentWarrior =
        bIsFemale ? CurrentWarriorLevel.GetFemaleWarrior() : CurrentWarriorLevel.GetMaleWarrior();
    GetMesh()->SetSkeletalMesh(CurrentWarrior.GetSkeletalMesh());
    GetMesh()->SetAnimInstanceClass(CurrentWarrior.GetAnimationBlueprint());
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentWarrior.GetHalfHeight());
    FVector NewMeshLoc = GetMesh()->GetRelativeLocation();
    NewMeshLoc.Z = -CurrentWarrior.GetHalfHeight();
    GetMesh()->SetRelativeLocation(NewMeshLoc);
    FVector NewBoxExtent = GetBoxCollider()->GetUnscaledBoxExtent();
    NewBoxExtent.Z = CurrentWarrior.GetHalfHeight();
    GetBoxCollider()->SetBoxExtent(NewBoxExtent);
    GetDefenderRadiusDisplay()->SetRelativeLocation(NewMeshLoc);
    HitFlashComponent->BindMaterials();
    if (bMustResumeFlash)
    {
        HitFlashComponent->SetRemainingFlashDuration(RemainingFlashDuration);
        HitFlashComponent->ResumeFlash();
    }
    OutlineComponent->BindMaterials();
    if (Level == 1)
    {
        if (IsValid(Shield))
        {
            Shield->AttachToSkeleton(GetMesh());
            Shield->GetMesh()->SetVisibility(true);
            Shield->GetMesh()->SetComponentTickEnabled(Shield->DoesMeshTick());
            if (bMustResumeFlash)
            {
                Shield->GetHitFlashComponent()->SetRemainingFlashDuration(RemainingFlashDuration);
                Shield->GetHitFlashComponent()->ResumeFlash();
            }
        }
    }
    if (AWarriorWeapon* Sword = GetWeapon(); IsValid(Sword))
    {
        Sword->AttachToSkeleton(GetMesh());
        Sword->GetMesh()->SetVisibility(true);
        Sword->GetMesh()->SetComponentTickEnabled(Sword->DoesMeshTick());
        if (bMustResumeFlash)
        {
            Sword->GetHitFlashComponent()->SetRemainingFlashDuration(RemainingFlashDuration);
            Sword->GetHitFlashComponent()->ResumeFlash();
        }
    }
    AttackRadius = CurrentWarriorLevel.GetAttackRadius();
    AttackCooldown = CurrentWarriorLevel.GetAttackCooldown();
    if (bShowingRadius) ShowRadiusDisplay(); // Update radius display to match any new values
    if (bShowingOutline) ShowOutline(); // Reshow outline because was hidden when unbound materials
}

FWarriorLevel AWarrior::GetWarriorCurrentLevel() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    if (CurrentLevel == 0) return WarriorBaseLevel;
    if (CurrentLevel == 1) return WarriorFirstUpgrade;
    return WarriorSecondUpgrade;
}

const AWarriorWeapon* AWarrior::GetWeapon() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    const UChildActorComponent* Weapon = CurrentLevel == 0 || CurrentLevel == 1 ? OneHandWeapon : TwoHandWeapon;
    return Cast<AWarriorWeapon>(Weapon ? Weapon->GetChildActor() : nullptr);
}

AWarriorWeapon* AWarrior::GetWeapon()
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    const UChildActorComponent* Weapon = CurrentLevel == 0 || CurrentLevel == 1 ? OneHandWeapon : TwoHandWeapon;
    return Cast<AWarriorWeapon>(Weapon ? Weapon->GetChildActor() : nullptr);
}

AWarriorWeapon* AWarrior::Blueprint_GetWeapon() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    const UChildActorComponent* Weapon = CurrentLevel == 0 || CurrentLevel == 1 ? OneHandWeapon : TwoHandWeapon;
    return Cast<AWarriorWeapon>(Weapon ? Weapon->GetChildActor() : nullptr);
}

AWarriorWeapon* AWarrior::GetOneHandSword() const
{
    return Cast<AWarriorWeapon>(OneHandWeapon ? OneHandWeapon->GetChildActor() : nullptr);
}

AWarriorShield* AWarrior::GetShield() const
{
    return Cast<AWarriorShield>(WarriorShield ? WarriorShield->GetChildActor() : nullptr);
}

AWarriorWeapon* AWarrior::GetTwoHandSword() const
{
    return Cast<AWarriorWeapon>(TwoHandWeapon ? TwoHandWeapon->GetChildActor() : nullptr);
}

void AWarrior::OnAttackMontageEnded(UAnimMontage*, bool) { EndAttack(); }

void AWarrior::OnDeathMontageEnded(UAnimMontage*, bool) { OnDeathComplete(); }
