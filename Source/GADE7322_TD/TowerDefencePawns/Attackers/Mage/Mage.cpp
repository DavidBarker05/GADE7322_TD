#include "TowerDefencePawns/Attackers/Mage/Mage.h"

#include "HitFlashComponent.h"
#include "NiagaraComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TowerDefenceGameMode.h"
#include "TowerDefencePawns/Attackers/Mage/AI/MageAIController.h"
#include "TowerDefencePawns/Attackers/Mage/DamageSpellProjectile.h"
#include "TowerDefencePawns/Components/DamageComponent.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"

AMage::AMage()
{
    PrimaryActorTick.bCanEverTick = true;
    PawnDisplayName = TEXT("Mage");
    OccupiedRadius = 40.0f;
    CurrentTeam = EAITeam::SupportAttacker;
    SpellSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spell Spawn Location"));
    SpellSpawnLocation->SetupAttachment(GetMesh(), "spell");
    BoostSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Boost Spell Ball"));
    DamageSpellBall = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Damage Spell Ball"));
}

void AMage::BeginPlay()
{
    Super::BeginPlay();
    BoostSpellBall->AttachToComponent(SpellSpawnLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    DamageSpellBall->AttachToComponent(SpellSpawnLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    HitFlashComponent->BindMaterials();
}

void AMage::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!IsPawnActive()) return;
    if (!IsValid(CurrentTarget) || !CurrentTarget->IsPawnActive() || CurrentTarget->GetHealthComponent()->IsDead())
    {
        if (bIsHoldingSpell)
        {
            BoostSpellBall->SetVisibility(false);
            BoostSpellBall->SetComponentTickEnabled(false);
            DamageSpellBall->SetVisibility(false);
            DamageSpellBall->SetComponentTickEnabled(false);
        }
        bIsHoldingSpell = false;
        TimeSinceLastVisionUpdate = 0.0f;
        StopBoosting();
        return;
    }
    if (TimeSinceLastVisionUpdate < 1.0f / SpellDistanceCheckUpdateFrequency + KINDA_SMALL_NUMBER)
    {
        TimeSinceLastVisionUpdate += DeltaTime;
        return;
    }
    TimeSinceLastVisionUpdate = 0.0f;
    const bool bOtherTargetFriendly = IsOtherPawnFriendly(CurrentTarget);
    const float Radius = (bOtherTargetFriendly ? BoostRadius : AttackRadius) * SpellShowRadiusMultiplier;
    const bool bWasHoldingSpell = bIsHoldingSpell;
    bIsHoldingSpell =
        FVector::Dist2D(GetActorLocation(), CurrentTarget->GetActorLocation()) <= Radius + KINDA_SMALL_NUMBER;
    if (bWasHoldingSpell != bIsHoldingSpell)
    {
        (bOtherTargetFriendly ? BoostSpellBall : DamageSpellBall)->SetVisibility(bIsHoldingSpell);
        (bOtherTargetFriendly ? BoostSpellBall : DamageSpellBall)->SetComponentTickEnabled(bIsHoldingSpell);
    }
}

void AMage::StartAttack()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    FOnMontageEnded EndDelegate;
    if (!IsCurrentTargetFriendly())
    {
        bCanAttack = false;
        AnimInstance->Montage_Play(AttackMontage, 1.0f);
        EndDelegate.BindUObject(this, &AMage::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
    }
    else if (!bBoostStarted && !bIsBoosting)
    {
        bBoostStarted = true;
        AnimInstance->Montage_Play(BoostStartMontage, 1.0f);
        EndDelegate.BindUObject(this, &AMage::OnBoostStartMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, BoostStartMontage);
    }
}

void AMage::Attack(ATowerDefencePawn* Other)
{
    if (!IsValid(Other) || !Other->IsPawnActive()) return;
    if (FVector::Dist2D(GetActorLocation(), Other->GetVisualAttackPointLocation()) <=
        AttackNoThrowingRadius + KINDA_SMALL_NUMBER)
        Super::Attack(Other);
    else if (PROJECTILE_POOL_FACTORY_EXISTS)
    {
        const FVector LaunchLocation = SpellSpawnLocation->GetComponentLocation();
        const FVector TargetLocation = Other->GetVisualAttackPointLocation();
        ADamageSpellProjectile* DamageSpell =
            Cast<ADamageSpellProjectile>(CREATE_PROJECTILE(DamageSpellClass, FTransform(LaunchLocation)));
        if (!DamageSpell) return;
        const FVector LaunchVelocity = (TargetLocation - LaunchLocation).GetSafeNormal() * DamageSpellSpeed;
        DamageSpell->Fire(Other->GetVisualAttackPoint(), DamageComponent->GetDamage(), LaunchVelocity);
    }
}

void AMage::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void AMage::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted EndDelegate;
    EndDelegate.BindUObject(this, &AMage::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(EndDelegate, DeathMontage);
}

void AMage::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    StopBoosting();
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
    BoostSpellBall->SetVisibility(false);
    BoostSpellBall->SetComponentTickEnabled(false);
    DamageSpellBall->SetVisibility(false);
    DamageSpellBall->SetComponentTickEnabled(false);
    TimeSinceLastVisionUpdate = 0.0f;
    bIsHoldingSpell = false;
    if (bActive)
    {
        GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
        GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
}

void AMage::DoUpdatePerceptionOnTeamChange()
{
    if (AMageAIController* M_AIC = GetController<AMageAIController>())
    {
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*M_AIC->GetAIPerceptionComponent());
        M_AIC->GetAIPerceptionComponent()->ForgetAll();
        M_AIC->GetVisiblePawns().Empty();
        M_AIC->GetVisibleEnemies().Empty();
        M_AIC->GetVisibleEnemies().Empty();
        CurrentTarget = nullptr;
    }
}

void AMage::SetCurrentTarget(ATowerDefencePawn* NewTarget)
{
    if (NewTarget != CurrentTarget) StopBoosting();
    const ATowerDefencePawn* OldTarget = CurrentTarget;
    const bool bOldTargetWasFriendly = bTargetIsFriendly;
    CurrentTarget = NewTarget;
    bTargetIsFriendly = IsOtherPawnFriendly(NewTarget);
    if (CurrentTarget && bIsHoldingSpell && OldTarget != CurrentTarget && bOldTargetWasFriendly != bTargetIsFriendly)
    {
        (bTargetIsFriendly ? DamageSpellBall : BoostSpellBall)->SetVisibility(false);
        (bTargetIsFriendly ? BoostSpellBall : DamageSpellBall)->SetVisibility(true);
        (bTargetIsFriendly ? DamageSpellBall : BoostSpellBall)->SetComponentTickEnabled(false);
        (bTargetIsFriendly ? BoostSpellBall : DamageSpellBall)->SetComponentTickEnabled(true);
    }
}

bool AMage::IsOtherPawnFriendly(const ATowerDefencePawn* OtherPawn) const
{
    if (IsValid(OtherPawn) && OtherPawn->IsPawnActive())
    {
        const ETeamAttitude::Type Attitude =
            ATowerDefenceGameMode::GetAttitudeCustom(CurrentTeam, OtherPawn->GetCurrentTeam());
        return Attitude == ETeamAttitude::Friendly;
    }
    return false;
}

void AMage::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) { EndAttack(); }

void AMage::OnBoostStartMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bBoostStarted = false;
    if (!IsValid(CurrentTarget) || !CurrentTarget->IsPawnActive() || CurrentTarget->GetHealthComponent()->IsDead() ||
        !bTargetIsFriendly)
        return;
    bIsBoosting = true;
    if (UDamageComponent* TargetDamage = CurrentTarget->GetDamageComponent())
    {
        if (TargetDamage->AddDamageBoost(DamageBoostAmount)) // Was the first booster
            if (AAttacker* TargetAttacker = Cast<AAttacker>(CurrentTarget))
                TargetAttacker->GetBoostEffect()->Activate(true);
    }
}

void AMage::StopBoosting()
{
    if (!bIsBoosting) return;
    bIsBoosting = false;
    if (!IsValid(CurrentTarget)) return;
    if (UDamageComponent* TargetDamage = CurrentTarget->GetDamageComponent())
    {
        if (TargetDamage->RemoveDamageBoost(DamageBoostAmount)) // Was the last booster
            if (AAttacker* TargetAttacker = Cast<AAttacker>(CurrentTarget))
                TargetAttacker->GetBoostEffect()->Deactivate();
    }
}

void AMage::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted) { OnDeathComplete(); }
