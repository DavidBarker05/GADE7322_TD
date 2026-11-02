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
    HealSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Heal Spell Ball"));
    FireSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Spell Ball"));
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
    if (!IsValid(CurrentTarget) || !CurrentTarget->IsPawnActive() || CurrentTarget->GetHealthComponent()->IsDead())
    {
        if (bIsHoldingSpell)
        {
            HealSpellBall->SetVisibility(false);
            HealSpellBall->SetComponentTickEnabled(false);
            FireSpellBall->SetVisibility(false);
            FireSpellBall->SetComponentTickEnabled(false);
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

void AHealer::Attack(ATowerDefencePawn* Other)
{
    if (!IsValid(Other) || !Other->IsPawnActive()) return;
    if (IsOtherPawnFriendly(Other))
    {
        auto Heal = [this](ADefender* Defender) -> void
        {
            Defender->GetHealthComponent()->ReceiveHealth(HealAmount);
            Defender->GetHealEffect()->Activate(true);
        };
        const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
        if (CurrentLevel == 2) // Heal in a radius
        {
            TArray<AActor*> OutActors;
            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
            ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
            if (UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), HealRadius, ObjectTypes,
                                                          ADefender::StaticClass(), {}, OutActors))
            {
                for (AActor* Actor : OutActors)
                {
                    ADefender* Defender = Cast<ADefender>(Actor);
                    if (!IsValid(Defender) || !Defender->IsPawnActive() || Defender->GetHealthComponent()->IsDead())
                        continue;
                    Heal(Defender);
                }
            }
        }
        else
        {
            ADefender* Defender = Cast<ADefender>(Other);
            if (!IsValid(Defender) || !Defender->IsPawnActive() || Defender->GetHealthComponent()->IsDead()) return;
            Heal(Defender);
        }
    }
    else
    {
        if (FVector::Dist2D(GetActorLocation(), Other->GetVisualAttackPointLocation()) <=
            AttackNoThrowingRadius + KINDA_SMALL_NUMBER)
            Super::Attack(Other);
        else if (PROJECTILE_POOL_FACTORY_EXISTS)
        {
            const FVector LaunchLocation = SpellSpawnLocation->GetComponentLocation();
            const FVector TargetLocation = Other->GetVisualAttackPointLocation();
            AFireballProjectile* Fireball =
                Cast<AFireballProjectile>(CREATE_PROJECTILE(FireballClass, FTransform(LaunchLocation)));
            if (!Fireball) return;
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
    if (bActive)
    {
        SetCurrentTarget(nullptr);
        GetUpgradeComponent()->SetLevel(0); // Reset to base level
    }
    else
    {
        SpellSpawnLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        HealSpellBall->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        FireSpellBall->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        HitFlashComponent->UnbindMaterials();
    }
    GetMesh()->SetVisibility(bActive);
    GetMesh()->SetComponentTickEnabled(bActive);
    GetMesh()->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToAllChannels(bActive ? ECR_Block : ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
    HealSpellBall->SetVisibility(false);
    HealSpellBall->SetComponentTickEnabled(false);
    FireSpellBall->SetVisibility(false);
    FireSpellBall->SetComponentTickEnabled(false);
    TimeSinceLastVisionUpdate = 0.0f;
    bIsHoldingSpell = false;
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
    const FDefenderLevel CurrentDefenderLevel = GetUpgradeComponent()->GetCurrent();
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
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->StopAllMontages(0.0f);
        AnimInstance->InitializeAnimation();
    }
    HitFlashComponent->BindMaterials();
    DamageComponent->SetDamage(CurrentDefenderLevel.GetDamage());
    HealthComponent->SetMaxHealth(CurrentDefenderLevel.GetHealth());
    HealRadius = CurrentHealerLevel.GetHealRadius();
    AttackRadius = CurrentHealerLevel.GetAttackRadius();
    HealRadius = CurrentHealerLevel.GetHealRadius();
    AttackCooldown = CurrentHealerLevel.GetAttackCooldown();
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
        (bTargetIsFriendly ? FireSpellBall : HealSpellBall)->SetVisibility(false);
        (bTargetIsFriendly ? HealSpellBall : FireSpellBall)->SetVisibility(true);
        (bTargetIsFriendly ? FireSpellBall : HealSpellBall)->SetComponentTickEnabled(false);
        (bTargetIsFriendly ? HealSpellBall : FireSpellBall)->SetComponentTickEnabled(true);
    }
}

FHealerLevel AHealer::GetHealerCurrentLevel() const
{
    const int32 CurrentLevel = GetUpgradeComponent()->GetCurrentLevel();
    if (CurrentLevel == 0) return HealerBaseLevel;
    if (CurrentLevel == 1) return HealerFirstUpgrade;
    return HealerSecondUpgrade;
}

bool AHealer::IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const
{
    if (IsValid(OtherPawn) && OtherPawn->IsPawnActive())
    {
        const ETeamAttitude::Type Attitude =
            ATowerDefenceGameMode::GetAttitudeCustom(CurrentTeam, OtherPawn->GetCurrentTeam());
        return Attitude == ETeamAttitude::Friendly;
    }
    return false;
}

void AHealer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) { EndAttack(); }

void AHealer::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted) { OnDeathComplete(); }
