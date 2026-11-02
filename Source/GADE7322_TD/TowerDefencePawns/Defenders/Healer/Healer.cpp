#include "TowerDefencePawns/Defenders/Healer/Healer.h"

#include "NiagaraComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TDCollisionChannels.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Defenders/Components/DefenderUpgradeComponent.h"
#include "TowerDefencePawns/Defenders/Healer/AI/HealerAIController.h"
#include "TowerDefencePawns/Defenders/Healer/FireballProjectile.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"

AHealer::AHealer()
{
    PrimaryActorTick.bCanEverTick = true;
    PawnDisplayName = TEXT("Healer");
    OccupiedRadius = 40.0f;
    CurrentTeam = EAITeam::SupportDefender;
    SpellSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spell Spawn Location"));
    SpellSpawnLocation->SetupAttachment(RootComponent);
    HealSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Heal Spell Ball"));
    HealSpellBall->SetupAttachment(RootComponent);
    FireSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Spell Ball"));
    FireSpellBall->SetupAttachment(RootComponent);
    SpellBook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spell Book"));
    SpellBook->SetupAttachment(RootComponent);
    PrimaryRadiusColour = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    SecondaryRadiusColour = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void AHealer::BeginPlay()
{
    Super::BeginPlay();
    TowerDefenceGameMode = Cast<ATowerDefenceGameMode>(UGameplayStatics::GetGameMode(this));
}

void AHealer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsPawnActive()) return;
    if (!IsPawnActiveAndAlive(CurrentTarget))
    {
        if (bIsHoldingSpell)
        {
            DisableSpellBall(HealSpellBall);
            DisableSpellBall(FireSpellBall);
        }
        bIsHoldingSpell = false;
        TimeSinceLastVisionUpdate = 0.0f;
        return;
    }
    if (TimeSinceLastVisionUpdate < 1.0f / SpellDistanceCheckUpdateFrequency + KINDA_SMALL_NUMBER)
    {
        TimeSinceLastVisionUpdate += DeltaTime;
        return;
    }
    TimeSinceLastVisionUpdate = 0.0f;
    const bool bOtherTargetFriendly = IsOtherPawnFriendly(CurrentTarget);
    const float Radius = (bOtherTargetFriendly ? HealRadius : AttackRadius) * SpellShowRadiusMultiplier;
    const bool bWasHoldingSpell = bIsHoldingSpell;
    bIsHoldingSpell =
        FVector::Dist2D(GetActorLocation(), CurrentTarget->GetActorLocation()) <= Radius + KINDA_SMALL_NUMBER;
    if (bWasHoldingSpell != bIsHoldingSpell)
    {
        (bOtherTargetFriendly ? HealSpellBall : FireSpellBall)->SetVisibility(bIsHoldingSpell);
        (bOtherTargetFriendly ? HealSpellBall : FireSpellBall)->SetComponentTickEnabled(bIsHoldingSpell);
    }
}

void AHealer::StartAttack()
{
    if (!TowerDefenceGameMode || !TowerDefenceGameMode->IsWaveInProgress())
        return; // Don't heal while a wave is in progress
    bCanAttack = false;
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    FHealerSkeletalMesh CurrentSkeletalMesh = GetHealerCurrentLevel().GetHealerSkeletalMesh();
    UAnimMontage* SpellMontage =
        bTargetIsFriendly ? CurrentSkeletalMesh.GetHealMontage() : CurrentSkeletalMesh.GetAttackMontage();
    AnimInstance->Montage_Play(SpellMontage, 1.0f);
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &AHealer::OnAttackMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, SpellMontage);
}

void AHealer::Heal(ATowerDefencePawn* Other)
{
    if (!IsPawnActiveAndAlive(Other)) return;
    Other->GetHealthComponent()->ReceiveHealth(HealAmount);
    if (ADefender* Defender = Cast<ADefender>(Other)) Defender->GetHealEffect()->Activate(true);
}

void AHealer::Attack(ATowerDefencePawn* Other)
{
    if (IsOtherPawnFriendly(Other))
    {
        const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
        if (CurrentLevel == 2) // Heal in a radius
        {
            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {UEngineTypes::ConvertToObjectType(ECC_Pawn)};
            TArray<AActor*> OutActors;
            if (UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), HealRadius, ObjectTypes,
                                                          ADefender::StaticClass(), {}, OutActors))
            {
                for (const auto Actor : OutActors) Heal(Cast<ATowerDefencePawn>(Actor));
            }
        }
        else Heal(Other);
    }
    else
    {
        if (FVector::Dist2D(GetActorLocation(), Other->GetVisualAttackPointLocation()) <=
            AttackNoThrowingRadius + KINDA_SMALL_NUMBER)
            Super::Attack(Other);
        else if (PROJECTILE_POOL_FACTORY_EXISTS)
        {
            const FVector LaunchLocation = SpellSpawnLocation->GetComponentLocation();
            AFireballProjectile* Fireball =
                Cast<AFireballProjectile>(CREATE_PROJECTILE(FireballClass, FTransform(LaunchLocation)));
            if (!Fireball) return;
            const FVector TargetLocation = Other->GetVisualAttackPointLocation();
            const FVector LaunchVelocity = (TargetLocation - LaunchLocation).GetSafeNormal() * FireballSpeed;
            Fireball->Fire(Other->GetVisualAttackPoint(), LaunchVelocity, DamageComponent->GetDamage());
        }
    }
}

void AHealer::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void AHealer::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage* DeathMontage = GetHealerCurrentLevel().GetHealerSkeletalMesh().GetDeathMontage();
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted EndDelegate;
    EndDelegate.BindUObject(this, &AHealer::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(EndDelegate, DeathMontage);
}

void AHealer::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    if (bActive) GetUpgradeComponent()->SetLevel(0); // Reset to base level
    SetCurrentTarget(nullptr);
    GetMesh()->SetVisibility(bActive);
    GetMesh()->SetComponentTickEnabled(bActive);
    GetMesh()->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    TimeSinceLastVisionUpdate = 0.0f;
    bIsHoldingSpell = false;
    DisableSpellBall(HealSpellBall);
    DisableSpellBall(FireSpellBall);
    if (bActive)
    {
        GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
}

void AHealer::DoUpdatePerceptionOnTeamChange()
{
    if (AHealerAIController* H_AIC = GetController<AHealerAIController>())
    {
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*H_AIC->GetAIPerceptionComponent());
        H_AIC->GetAIPerceptionComponent()->ForgetAll();
        H_AIC->GetVisiblePawns().Empty();
        H_AIC->GetVisibleEnemies().Empty();
        H_AIC->GetVisibleEnemies().Empty();
        CurrentTarget = nullptr;
    }
}

void AHealer::DoOnLevelChanged(int32 Level)
{
    // vvv Reset animation, detach held items and unbind materials because the mesh is about to change vvv
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->StopAllMontages(0.0f);
        AnimInstance->InitializeAnimation();
    }
    SpellSpawnLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    HealSpellBall->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    FireSpellBall->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    SpellBook->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    SpellBook->SetVisibility(false);
    SpellBook->SetComponentTickEnabled(false);
    HitFlashComponent->UnbindMaterials();
    // ^^^ Reset animation, detach held items and unbind materials because the mesh is about to change ^^^

    const FHealerLevel CurrentHealerLevel = GetHealerCurrentLevel();
    FHealerSkeletalMesh CurrentSkeletalMesh = CurrentHealerLevel.GetHealerSkeletalMesh();
    GetMesh()->SetSkeletalMesh(CurrentSkeletalMesh.GetSkeletalMesh());
    GetMesh()->SetAnimInstanceClass(CurrentSkeletalMesh.GetAnimationBlueprint());
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentSkeletalMesh.GetHalfHeight());
    FVector NewMeshLoc = GetMesh()->GetRelativeLocation();
    NewMeshLoc.Z = -CurrentSkeletalMesh.GetHalfHeight();
    GetMesh()->SetRelativeLocation(NewMeshLoc);
    FVector NewBoxExtent = GetBoxCollider()->GetUnscaledBoxExtent();
    NewBoxExtent.Z = CurrentSkeletalMesh.GetHalfHeight();
    GetBoxCollider()->SetBoxExtent(NewBoxExtent);
    SpellSpawnLocation->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                                          TEXT("spell"));
    HealSpellBall->AttachToComponent(SpellSpawnLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    FireSpellBall->AttachToComponent(SpellSpawnLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    if (Level == 1)
    {
        SpellBook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "spellBook");
        SpellBook->SetVisibility(true);
        SpellBook->SetComponentTickEnabled(true);
    }
    HitFlashComponent->BindMaterials();
    HealRadius = CurrentHealerLevel.GetHealRadius();
    AttackRadius = CurrentHealerLevel.GetAttackRadius();
    HealAmount = CurrentHealerLevel.GetHealAmount();
}

void AHealer::SetCurrentTarget(ATowerDefencePawn* NewTarget)
{
    const ATowerDefencePawn* OldTarget = CurrentTarget;
    const bool bOldTargetWasFriendly = bTargetIsFriendly;
    CurrentTarget = NewTarget;
    bTargetIsFriendly = IsOtherPawnFriendly(NewTarget);
    if (CurrentTarget && bIsHoldingSpell && OldTarget != CurrentTarget && bOldTargetWasFriendly != bTargetIsFriendly)
    {
        DisableSpellBall(bTargetIsFriendly ? FireSpellBall : HealSpellBall);
        EnableSpellBall(bTargetIsFriendly ? HealSpellBall : FireSpellBall);
    }
}

FHealerLevel AHealer::GetHealerCurrentLevel() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    if (CurrentLevel == 0) return HealerBaseLevel;
    if (CurrentLevel == 1) return HealerFirstUpgrade;
    return HealerSecondUpgrade;
}

void AHealer::EnableSpellBall(UNiagaraComponent* Spell)
{
    Spell->SetVisibility(true);
    Spell->SetComponentTickEnabled(true);
}

void AHealer::DisableSpellBall(UNiagaraComponent* Spell)
{
    Spell->SetVisibility(false);
    Spell->SetComponentTickEnabled(false);
}

void AHealer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) { EndAttack(); }

void AHealer::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted) { OnDeathComplete(); }
