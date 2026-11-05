#include "TowerDefencePawns/Attackers/ProceduralEnemy/Golem.h"

#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "DamageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/Animation/GolemAnimator.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemArm.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemBody.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemLeg.h"
#include "TowerDefencePawns/Components/HitFlashComponent.h"

AGolem::AGolem()
{
    PawnDisplayName = TEXT("Golem");
    OccupiedRadius = 50.0f;
    CurrentTeam = EAITeam::MeleeAttacker;
    Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
    Head->SetupAttachment(RootComponent);
    Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Head->SetCanEverAffectNavigation(false);
    Animator = CreateDefaultSubobject<UGolemAnimator>(TEXT("Animator"));
    WeakRightArmComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weak Right Arm Component"));
    WeakRightArmComponent->SetupAttachment(RootComponent);
    AverageRightArmComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Average Right Arm Component"));
    AverageRightArmComponent->SetupAttachment(RootComponent);
    StrongRightArmComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Strong Right Arm Component"));
    StrongRightArmComponent->SetupAttachment(RootComponent);
    WeakLeftArmComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weak Left Arm Component"));
    WeakLeftArmComponent->SetupAttachment(RootComponent);
    AverageLeftArmComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Average Left Arm Component"));
    AverageLeftArmComponent->SetupAttachment(RootComponent);
    StrongLeftArmComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Strong Left Arm Component"));
    StrongLeftArmComponent->SetupAttachment(RootComponent);
    WeakBodyComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weak Body Component"));
    WeakBodyComponent->SetupAttachment(RootComponent);
    AverageBodyComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Average Body Component"));
    AverageBodyComponent->SetupAttachment(RootComponent);
    StrongBodyComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Strong Body Component"));
    StrongBodyComponent->SetupAttachment(RootComponent);
    WeakRightLegComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weak Right Leg Component"));
    WeakRightLegComponent->SetupAttachment(RootComponent);
    AverageRightLegComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Average Right Leg Component"));
    AverageRightLegComponent->SetupAttachment(RootComponent);
    StrongRightLegComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Strong Right Leg Component"));
    StrongRightLegComponent->SetupAttachment(RootComponent);
    WeakLeftLegComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weak Left Leg Component"));
    WeakLeftLegComponent->SetupAttachment(RootComponent);
    AverageLeftLegComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Average Left Leg Component"));
    AverageLeftLegComponent->SetupAttachment(RootComponent);
    StrongLeftLegComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Strong Left Leg Component"));
    StrongLeftLegComponent->SetupAttachment(RootComponent);
}

void AGolem::BeginPlay()
{
    Super::BeginPlay();
    Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Head->SetCanEverAffectNavigation(false);
    Head->SetGenerateOverlapEvents(false);
    Animator->OnAttackHit.AddUObject(this, &AGolem::OnAnimatorAttackHit);
    Animator->OnAttackFinished.AddUObject(this, &AGolem::OnAnimatorAttackFinished);
    HitFlashComponent->BindMaterials();
    if (AGolemArm* WeakRightArm = GetWeakRightArm(); IsValid(WeakRightArm))
        WeakRightArm->GetHitFlashComponent()->BindMaterials();
    if (AGolemArm* AverageRightArm = GetAverageRightArm(); IsValid(AverageRightArm))
        AverageRightArm->GetHitFlashComponent()->BindMaterials();
    if (AGolemArm* StrongRightArm = GetStrongRightArm(); IsValid(StrongRightArm))
        StrongRightArm->GetHitFlashComponent()->BindMaterials();
    if (AGolemArm* WeakLeftArm = GetWeakLeftArm(); IsValid(WeakLeftArm))
        WeakLeftArm->GetHitFlashComponent()->BindMaterials();
    if (AGolemArm* AverageLeftArm = GetAverageLeftArm(); IsValid(AverageLeftArm))
        AverageLeftArm->GetHitFlashComponent()->BindMaterials();
    if (AGolemArm* StrongLeftArm = GetStrongLeftArm(); IsValid(StrongLeftArm))
        StrongLeftArm->GetHitFlashComponent()->BindMaterials();
    if (AGolemBody* WeakBody = GetWeakBody(); IsValid(WeakBody)) WeakBody->GetHitFlashComponent()->BindMaterials();
    if (AGolemBody* AverageBody = GetAverageBody(); IsValid(AverageBody))
        AverageBody->GetHitFlashComponent()->BindMaterials();
    if (AGolemBody* StrongBody = GetStrongBody(); IsValid(StrongBody))
        StrongBody->GetHitFlashComponent()->BindMaterials();
    if (AGolemLeg* WeakRightLeg = GetWeakRightLeg(); IsValid(WeakRightLeg))
        WeakRightLeg->GetHitFlashComponent()->BindMaterials();
    if (AGolemLeg* AverageRightLeg = GetAverageRightLeg(); IsValid(AverageRightLeg))
        AverageRightLeg->GetHitFlashComponent()->BindMaterials();
    if (AGolemLeg* StrongRightLeg = GetStrongRightLeg(); IsValid(StrongRightLeg))
        StrongRightLeg->GetHitFlashComponent()->BindMaterials();
    if (AGolemLeg* WeakLeftLeg = GetWeakLeftLeg(); IsValid(WeakLeftLeg))
        WeakLeftLeg->GetHitFlashComponent()->BindMaterials();
    if (AGolemLeg* AverageLeftLeg = GetAverageLeftLeg(); IsValid(AverageLeftLeg))
        AverageLeftLeg->GetHitFlashComponent()->BindMaterials();
    if (AGolemLeg* StrongLeftLeg = GetStrongLeftLeg(); IsValid(StrongLeftLeg))
        StrongLeftLeg->GetHitFlashComponent()->BindMaterials();
}

void AGolem::DoFlash()
{
    Super::DoFlash();
    if (IsValid(CurrentRightArm)) CurrentRightArm->GetHitFlashComponent()->DoFlash();
    if (IsValid(CurrentLeftArm)) CurrentLeftArm->GetHitFlashComponent()->DoFlash();
    if (IsValid(CurrentBody)) CurrentBody->GetHitFlashComponent()->DoFlash();
    if (IsValid(CurrentRightLeg)) CurrentRightLeg->GetHitFlashComponent()->DoFlash();
    if (IsValid(CurrentLeftLeg)) CurrentLeftLeg->GetHitFlashComponent()->DoFlash();
}

EGolemPartStrength AGolem::GetRandomPartStrength()
{
    switch (FMath::RandRange(0, 2))
    {
        case 0:
            return EGolemPartStrength::Weak;
        case 1:
            return EGolemPartStrength::Average;
        case 2:
            return EGolemPartStrength::Strong;
        default:
            return EGolemPartStrength::Unknown;
    }
}

const AGolemArm* AGolem::GetRightArm(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakRightArm();
        case EGolemPartStrength::Average:
            return GetAverageRightArm();
        case EGolemPartStrength::Strong:
            return GetStrongRightArm();
        default:
            return nullptr;
    }
}

AGolemArm* AGolem::GetRightArm(EGolemPartStrength Strength)
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakRightArm();
        case EGolemPartStrength::Average:
            return GetAverageRightArm();
        case EGolemPartStrength::Strong:
            return GetStrongRightArm();
        default:
            return nullptr;
    }
}

AGolemArm* AGolem::GetRightArm_Blueprint(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakRightArm_Blueprint();
        case EGolemPartStrength::Average:
            return GetAverageRightArm_Blueprint();
        case EGolemPartStrength::Strong:
            return GetStrongRightArm_Blueprint();
        default:
            return nullptr;
    }
}

const AGolemArm* AGolem::GetLeftArm(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakLeftArm();
        case EGolemPartStrength::Average:
            return GetAverageLeftArm();
        case EGolemPartStrength::Strong:
            return GetStrongLeftArm();
        default:
            return nullptr;
    }
}

AGolemArm* AGolem::GetLeftArm(EGolemPartStrength Strength)
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakLeftArm();
        case EGolemPartStrength::Average:
            return GetAverageLeftArm();
        case EGolemPartStrength::Strong:
            return GetStrongLeftArm();
        default:
            return nullptr;
    }
}

AGolemArm* AGolem::GetLeftArm_Blueprint(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakLeftArm_Blueprint();
        case EGolemPartStrength::Average:
            return GetAverageLeftArm_Blueprint();
        case EGolemPartStrength::Strong:
            return GetStrongLeftArm_Blueprint();
        default:
            return nullptr;
    }
}

const AGolemBody* AGolem::GetBody(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakBody();
        case EGolemPartStrength::Average:
            return GetAverageBody();
        case EGolemPartStrength::Strong:
            return GetStrongBody();
        default:
            return nullptr;
    }
}

AGolemBody* AGolem::GetBody(EGolemPartStrength Strength)
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakBody();
        case EGolemPartStrength::Average:
            return GetAverageBody();
        case EGolemPartStrength::Strong:
            return GetStrongBody();
        default:
            return nullptr;
    }
}

AGolemBody* AGolem::GetBody_Blueprint(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakBody_Blueprint();
        case EGolemPartStrength::Average:
            return GetAverageBody_Blueprint();
        case EGolemPartStrength::Strong:
            return GetStrongBody_Blueprint();
        default:
            return nullptr;
    }
}

const AGolemLeg* AGolem::GetRightLeg(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakRightLeg();
        case EGolemPartStrength::Average:
            return GetAverageRightLeg();
        case EGolemPartStrength::Strong:
            return GetStrongRightLeg();
        default:
            return nullptr;
    }
}

AGolemLeg* AGolem::GetRightLeg(EGolemPartStrength Strength)
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakRightLeg();
        case EGolemPartStrength::Average:
            return GetAverageRightLeg();
        case EGolemPartStrength::Strong:
            return GetStrongRightLeg();
        default:
            return nullptr;
    }
}

AGolemLeg* AGolem::GetRightLeg_Blueprint(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakRightLeg_Blueprint();
        case EGolemPartStrength::Average:
            return GetAverageRightLeg_Blueprint();
        case EGolemPartStrength::Strong:
            return GetStrongRightLeg_Blueprint();
        default:
            return nullptr;
    }
}

const AGolemLeg* AGolem::GetLeftLeg(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakLeftLeg();
        case EGolemPartStrength::Average:
            return GetAverageLeftLeg();
        case EGolemPartStrength::Strong:
            return GetStrongLeftLeg();
        default:
            return nullptr;
    }
}

AGolemLeg* AGolem::GetLeftLeg(EGolemPartStrength Strength)
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakLeftLeg();
        case EGolemPartStrength::Average:
            return GetAverageLeftLeg();
        case EGolemPartStrength::Strong:
            return GetStrongLeftLeg();
        default:
            return nullptr;
    }
}

AGolemLeg* AGolem::GetLeftLeg_Blueprint(EGolemPartStrength Strength) const
{
    switch (Strength)
    {
        case EGolemPartStrength::Weak:
            return GetWeakLeftLeg_Blueprint();
        case EGolemPartStrength::Average:
            return GetAverageLeftLeg_Blueprint();
        case EGolemPartStrength::Strong:
            return GetStrongLeftLeg_Blueprint();
        default:
            return nullptr;
    }
}

void AGolem::StartAttack()
{
    if (!bCanAttack || bDeathStarted) return;
    bCanAttack = false;
    Animator->PlayAttack();
}

void AGolem::EndAttack()
{
    GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
    if (bDeathStarted) return;
    if (AttackCooldown <= 0.0f)
    {
        bCanAttack = true;
        return;
    }
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void AGolem::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
    Animator->Freeze();
    GetCharacterMovement()->DisableMovement();
    StartCollapse();
    GetWorldTimerManager().SetTimer(CollapseTimerHandle, this, &AGolem::FinishCollapse,
                                    FMath::Max(CollapseDuration, 0.01f), false);
}

void AGolem::OnAnimatorAttackHit()
{
    if (bDeathStarted) return;
    if (IsPawnActiveAndAlive(CurrentAttackTarget)) Attack(CurrentAttackTarget);
}

void AGolem::OnAnimatorAttackFinished() { EndAttack(); }

void AGolem::FinishCollapse()
{
    StopCollapse();
    OnDeathComplete();
}

void AGolem::StartCollapse()
{
    if (IsValid(CurrentBody)) CurrentBody->StartCollapse(CollapseImpulse, CollapseSpin);
    if (IsValid(CurrentRightArm)) CurrentRightArm->StartCollapse(CollapseImpulse, CollapseSpin);
    if (IsValid(CurrentLeftArm)) CurrentLeftArm->StartCollapse(CollapseImpulse, CollapseSpin);
    if (IsValid(CurrentRightLeg)) CurrentRightLeg->StartCollapse(CollapseImpulse, CollapseSpin);
    if (IsValid(CurrentLeftLeg)) CurrentLeftLeg->StartCollapse(CollapseImpulse, CollapseSpin);
    HeadCollapse.Begin({Head}, CollapseImpulse, CollapseSpin);
}

void AGolem::StopCollapse()
{
    GetWorldTimerManager().ClearTimer(CollapseTimerHandle);
    HeadCollapse.End();
    if (IsValid(CurrentBody)) CurrentBody->StopCollapse();
    if (IsValid(CurrentRightArm)) CurrentRightArm->StopCollapse();
    if (IsValid(CurrentLeftArm)) CurrentLeftArm->StopCollapse();
    if (IsValid(CurrentRightLeg)) CurrentRightLeg->StopCollapse();
    if (IsValid(CurrentLeftLeg)) CurrentLeftLeg->StopCollapse();
}

void AGolem::SetCurrentRightArm(AGolemArm* Arm) { CurrentRightArm = Arm; }

void AGolem::SetCurrentLeftArm(AGolemArm* Arm) { CurrentLeftArm = Arm; }

void AGolem::SetCurrentBody(AGolemBody* Body) { CurrentBody = Body; }

void AGolem::SetCurrentRightLeg(AGolemLeg* Leg) { CurrentRightLeg = Leg; }

void AGolem::SetCurrentLeftLeg(AGolemLeg* Leg) { CurrentLeftLeg = Leg; }

void AGolem::DoOnSetActive(bool bActive)
{
    Super::DoOnSetActive(bActive);
    StopCollapse();
    Animator->Stop();
    GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
    CurrentAttackTarget = nullptr;
    bCanAttack = true;
    auto Deactivate = [this]() -> void
    {
        Head->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        Head->SetVisibility(false);
        Head->SetComponentTickEnabled(false);
        if (AGolemArm* WeakRightArm = GetWeakRightArm(); IsValid(WeakRightArm))
        {
            WeakRightArm->DisableMesh();
            WeakRightArm->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemArm* AverageRightArm = GetAverageRightArm(); IsValid(AverageRightArm))
        {
            AverageRightArm->DisableMesh();
            AverageRightArm->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemArm* StrongRightArm = GetStrongRightArm(); IsValid(StrongRightArm))
        {
            StrongRightArm->DisableMesh();
            StrongRightArm->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemArm* WeakLeftArm = GetWeakLeftArm(); IsValid(WeakLeftArm))
        {
            WeakLeftArm->DisableMesh();
            WeakLeftArm->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemArm* AverageLeftArm = GetAverageLeftArm(); IsValid(AverageLeftArm))
        {
            AverageLeftArm->DisableMesh();
            AverageLeftArm->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemArm* StrongLeftArm = GetStrongLeftArm(); IsValid(StrongLeftArm))
        {
            StrongLeftArm->DisableMesh();
            StrongLeftArm->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemBody* WeakBody = GetWeakBody(); IsValid(WeakBody))
        {
            WeakBody->DisableMesh();
            WeakBody->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemBody* AverageBody = GetAverageBody(); IsValid(AverageBody))
        {
            AverageBody->DisableMesh();
            AverageBody->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemBody* StrongBody = GetStrongBody(); IsValid(StrongBody))
        {
            StrongBody->DisableMesh();
            StrongBody->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemLeg* WeakRightLeg = GetWeakRightLeg(); IsValid(WeakRightLeg))
        {
            WeakRightLeg->DisableMesh();
            WeakRightLeg->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemLeg* AverageRightLeg = GetAverageRightLeg(); IsValid(AverageRightLeg))
        {
            AverageRightLeg->DisableMesh();
            AverageRightLeg->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemLeg* StrongRightLeg = GetStrongRightLeg(); IsValid(StrongRightLeg))
        {
            StrongRightLeg->DisableMesh();
            StrongRightLeg->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemLeg* WeakLeftLeg = GetWeakLeftLeg(); IsValid(WeakLeftLeg))
        {
            WeakLeftLeg->DisableMesh();
            WeakLeftLeg->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemLeg* AverageLeftLeg = GetAverageLeftLeg(); IsValid(AverageLeftLeg))
        {
            AverageLeftLeg->DisableMesh();
            AverageLeftLeg->GetHitFlashComponent()->ForceStopFlash();
        }
        if (AGolemLeg* StrongLeftLeg = GetStrongLeftLeg(); IsValid(StrongLeftLeg))
        {
            StrongLeftLeg->DisableMesh();
            StrongLeftLeg->GetHitFlashComponent()->ForceStopFlash();
        }
        HitFlashComponent->ForceStopFlash();
    };
    if (bActive)
    {
        const EGolemPartStrength ArmStrength = GetRandomPartStrength();
        const EGolemPartStrength BodyStrength = GetRandomPartStrength();
        const EGolemPartStrength LegStrength = GetRandomPartStrength();
        AGolemArm *RightArm = GetRightArm(ArmStrength), *LeftArm = GetLeftArm(ArmStrength);
        AGolemBody* Body = GetBody(BodyStrength);
        AGolemLeg *RightLeg = GetRightLeg(LegStrength), *LeftLeg = GetLeftLeg(LegStrength);
        FString InvalidParts {};
        bool bTerminate = false;
        auto Validate = [&InvalidParts, &bTerminate](const AActor* Part, const FString& Name) -> void
        {
            if (!IsValid(Part))
            {
                if (InvalidParts.IsEmpty())
                {
                    InvalidParts.Append(Name);
                    bTerminate = true;
                }
                else
                {
                    InvalidParts.Append(TEXT(", "));
                    InvalidParts.Append(Name);
                }
            }
        };
        Validate(RightArm, TEXT("Right Arm"));
        Validate(LeftArm, TEXT("Left Arm"));
        Validate(Body, TEXT("Body"));
        Validate(RightLeg, TEXT("Right Leg"));
        Validate(LeftLeg, TEXT("Left Leg"));
        if (bTerminate)
        {
            TD_LOG_FATAL(
                TEXT("AGolem::DoOnSetActive -> Failed to create a golem because the following parts were invalid: %s"),
                *InvalidParts);
            Deactivate();
            return;
        }
        Head->AttachToComponent(Body->GetHeadAttachmentPoint(),
                                FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        Head->SetVisibility(true);
        Head->SetComponentTickEnabled(true);
        RightArm->AttachToComponent(Body->GetRightArmAttachmentPoint(),
                                    FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        RightArm->EnableMesh();
        CurrentRightArm = RightArm;
        LeftArm->AttachToComponent(Body->GetLeftArmAttachmentPoint(),
                                   FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        LeftArm->EnableMesh();
        CurrentLeftArm = LeftArm;
        Body->EnableMesh();
        CurrentBody = Body;
        RightLeg->AttachToComponent(Body->GetRightLegAttachmentPoint(),
                                    FAttachmentTransformRules::SnapToTargetIncludingScale);
        RightLeg->EnableMesh();
        CurrentRightLeg = RightLeg;
        LeftLeg->AttachToComponent(Body->GetLeftLegAttachmentPoint(),
                                   FAttachmentTransformRules::SnapToTargetIncludingScale);
        LeftLeg->EnableMesh();
        CurrentLeftLeg = LeftLeg;
        HealthComponent->SetBaseMaxHealth(CurrentBody->GetHealth());
        DamageComponent->SetDamage(CurrentRightArm->GetDamage());
        GetCharacterMovement()->MaxWalkSpeed = CurrentRightLeg->GetSpeed();
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        Animator->Setup(CurrentBody, CurrentRightArm, CurrentLeftArm, CurrentRightLeg, CurrentLeftLeg);
        ResetHealth();
    }
    else Deactivate();
}

const AGolemArm* AGolem::GetWeakRightArm() const
{
    return Cast<AGolemArm>(WeakRightArmComponent ? WeakRightArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetWeakRightArm()
{
    return Cast<AGolemArm>(WeakRightArmComponent ? WeakRightArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetWeakRightArm_Blueprint() const
{
    return Cast<AGolemArm>(WeakRightArmComponent ? WeakRightArmComponent->GetChildActor() : nullptr);
}

const AGolemArm* AGolem::GetAverageRightArm() const
{
    return Cast<AGolemArm>(AverageRightArmComponent ? AverageRightArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetAverageRightArm()
{
    return Cast<AGolemArm>(AverageRightArmComponent ? AverageRightArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetAverageRightArm_Blueprint() const
{
    return Cast<AGolemArm>(AverageRightArmComponent ? AverageRightArmComponent->GetChildActor() : nullptr);
}

const AGolemArm* AGolem::GetStrongRightArm() const
{
    return Cast<AGolemArm>(StrongRightArmComponent ? StrongRightArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetStrongRightArm()
{
    return Cast<AGolemArm>(StrongRightArmComponent ? StrongRightArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetStrongRightArm_Blueprint() const
{
    return Cast<AGolemArm>(StrongRightArmComponent ? StrongRightArmComponent->GetChildActor() : nullptr);
}

const AGolemArm* AGolem::GetWeakLeftArm() const
{
    return Cast<AGolemArm>(WeakLeftArmComponent ? WeakLeftArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetWeakLeftArm()
{
    return Cast<AGolemArm>(WeakLeftArmComponent ? WeakLeftArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetWeakLeftArm_Blueprint() const
{
    return Cast<AGolemArm>(WeakLeftArmComponent ? WeakLeftArmComponent->GetChildActor() : nullptr);
}

const AGolemArm* AGolem::GetAverageLeftArm() const
{
    return Cast<AGolemArm>(AverageLeftArmComponent ? AverageLeftArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetAverageLeftArm()
{
    return Cast<AGolemArm>(AverageLeftArmComponent ? AverageLeftArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetAverageLeftArm_Blueprint() const
{
    return Cast<AGolemArm>(AverageLeftArmComponent ? AverageLeftArmComponent->GetChildActor() : nullptr);
}

const AGolemArm* AGolem::GetStrongLeftArm() const
{
    return Cast<AGolemArm>(StrongLeftArmComponent ? StrongLeftArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetStrongLeftArm()
{
    return Cast<AGolemArm>(StrongLeftArmComponent ? StrongLeftArmComponent->GetChildActor() : nullptr);
}

AGolemArm* AGolem::GetStrongLeftArm_Blueprint() const
{
    return Cast<AGolemArm>(StrongLeftArmComponent ? StrongLeftArmComponent->GetChildActor() : nullptr);
}

const AGolemBody* AGolem::GetWeakBody() const
{
    return Cast<AGolemBody>(WeakBodyComponent ? WeakBodyComponent->GetChildActor() : nullptr);
}

AGolemBody* AGolem::GetWeakBody()
{
    return Cast<AGolemBody>(WeakBodyComponent ? WeakBodyComponent->GetChildActor() : nullptr);
}

AGolemBody* AGolem::GetWeakBody_Blueprint() const
{
    return Cast<AGolemBody>(WeakBodyComponent ? WeakBodyComponent->GetChildActor() : nullptr);
}

const AGolemBody* AGolem::GetAverageBody() const
{
    return Cast<AGolemBody>(AverageBodyComponent ? AverageBodyComponent->GetChildActor() : nullptr);
}

AGolemBody* AGolem::GetAverageBody()
{
    return Cast<AGolemBody>(AverageBodyComponent ? AverageBodyComponent->GetChildActor() : nullptr);
}

AGolemBody* AGolem::GetAverageBody_Blueprint() const
{
    return Cast<AGolemBody>(AverageBodyComponent ? AverageBodyComponent->GetChildActor() : nullptr);
}

const AGolemBody* AGolem::GetStrongBody() const
{
    return Cast<AGolemBody>(StrongBodyComponent ? StrongBodyComponent->GetChildActor() : nullptr);
}

AGolemBody* AGolem::GetStrongBody()
{
    return Cast<AGolemBody>(StrongBodyComponent ? StrongBodyComponent->GetChildActor() : nullptr);
}

AGolemBody* AGolem::GetStrongBody_Blueprint() const
{
    return Cast<AGolemBody>(StrongBodyComponent ? StrongBodyComponent->GetChildActor() : nullptr);
}

const AGolemLeg* AGolem::GetWeakRightLeg() const
{
    return Cast<AGolemLeg>(WeakRightLegComponent ? WeakRightLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetWeakRightLeg()
{
    return Cast<AGolemLeg>(WeakRightLegComponent ? WeakRightLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetWeakRightLeg_Blueprint() const
{
    return Cast<AGolemLeg>(WeakRightLegComponent ? WeakRightLegComponent->GetChildActor() : nullptr);
}

const AGolemLeg* AGolem::GetAverageRightLeg() const
{
    return Cast<AGolemLeg>(AverageRightLegComponent ? AverageRightLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetAverageRightLeg()
{
    return Cast<AGolemLeg>(AverageRightLegComponent ? AverageRightLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetAverageRightLeg_Blueprint() const
{
    return Cast<AGolemLeg>(AverageRightLegComponent ? AverageRightLegComponent->GetChildActor() : nullptr);
}

const AGolemLeg* AGolem::GetStrongRightLeg() const
{
    return Cast<AGolemLeg>(StrongRightLegComponent ? StrongRightLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetStrongRightLeg()
{
    return Cast<AGolemLeg>(StrongRightLegComponent ? StrongRightLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetStrongRightLeg_Blueprint() const
{
    return Cast<AGolemLeg>(StrongRightLegComponent ? StrongRightLegComponent->GetChildActor() : nullptr);
}

const AGolemLeg* AGolem::GetWeakLeftLeg() const
{
    return Cast<AGolemLeg>(WeakLeftLegComponent ? WeakLeftLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetWeakLeftLeg()
{
    return Cast<AGolemLeg>(WeakLeftLegComponent ? WeakLeftLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetWeakLeftLeg_Blueprint() const
{
    return Cast<AGolemLeg>(WeakLeftLegComponent ? WeakLeftLegComponent->GetChildActor() : nullptr);
}

const AGolemLeg* AGolem::GetAverageLeftLeg() const
{
    return Cast<AGolemLeg>(AverageLeftLegComponent ? AverageLeftLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetAverageLeftLeg()
{
    return Cast<AGolemLeg>(AverageLeftLegComponent ? AverageLeftLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetAverageLeftLeg_Blueprint() const
{
    return Cast<AGolemLeg>(AverageLeftLegComponent ? AverageLeftLegComponent->GetChildActor() : nullptr);
}

const AGolemLeg* AGolem::GetStrongLeftLeg() const
{
    return Cast<AGolemLeg>(StrongLeftLegComponent ? StrongLeftLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetStrongLeftLeg()
{
    return Cast<AGolemLeg>(StrongLeftLegComponent ? StrongLeftLegComponent->GetChildActor() : nullptr);
}

AGolemLeg* AGolem::GetStrongLeftLeg_Blueprint() const
{
    return Cast<AGolemLeg>(StrongLeftLegComponent ? StrongLeftLegComponent->GetChildActor() : nullptr);
}
