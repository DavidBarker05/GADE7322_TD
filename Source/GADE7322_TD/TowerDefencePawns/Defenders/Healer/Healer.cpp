#include "TowerDefencePawns/Defenders/Healer/Healer.h"

#include "HitFlashComponent.h"
#include "NiagaraComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
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
    SpellSpawnLocation->SetupAttachment(GetMesh(), "spell");
    HealSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Heal Spell Ball"));
    FireSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Spell Ball"));
    PrimaryRadiusColour = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    SecondaryRadiusColour = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void AHealer::BeginPlay()
{
    Super::BeginPlay();
    HealSpellBall->AttachToComponent(SpellSpawnLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    FireSpellBall->AttachToComponent(SpellSpawnLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    TowerDefenceGameMode = Cast<ATowerDefenceGameMode>(UGameplayStatics::GetGameMode(this));
    HitFlashComponent->BindMaterials();
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
    UAnimMontage* SpellMontage = bTargetIsFriendly ? HealMontage : AttackMontage;
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
        Other->GetHealthComponent()->ReceiveHealth(HealAmount);
        if (ADefender* Defender = Cast<ADefender>(Other)) Defender->GetHealEffect()->Activate(true);
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
            Fireball->Fire(Other->GetVisualAttackPoint(), DamageComponent->GetDamage(), LaunchVelocity);
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
