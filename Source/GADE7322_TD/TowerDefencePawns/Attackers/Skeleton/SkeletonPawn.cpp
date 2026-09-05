// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"

#include "Animation/AnimInstance.h"
#include "Components/ChildActorComponent.h"
#include "HitFlashComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "TowerDefencePawns/Attackers/Skeleton/AI/SkeletonAIController.h"
#include "Weapon.h"

ASkeletonPawn::ASkeletonPawn()
{
    PawnDisplayName = TEXT("Skeleton");
    OccupiedRadius = 40.0f;
    Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
    Weapon->SetupAttachment(RootComponent);
    CurrentTeam = EAITeam::Attacker;
}

void ASkeletonPawn::BeginPlay()
{
    Super::BeginPlay();
    HitFlashComponent->BindMaterials();
}

const AWeapon* ASkeletonPawn::GetWeapon() const { return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr); }

AWeapon* ASkeletonPawn::GetWeapon() { return Cast<AWeapon>(Weapon ? Weapon->GetChildActor() : nullptr); }

void ASkeletonPawn::StartAttack()
{
    bCanAttack = false;
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(AttackMontage, 1.0f);
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &ASkeletonPawn::OnAttackMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void ASkeletonPawn::EndAttack()
{
    GetWorldTimerManager().SetTimer(
        AttackCooldownHandle, [this]() -> void { bCanAttack = true; }, AttackCooldown, false);
}

void ASkeletonPawn::OnDeath(TFunction<void()>&& Func)
{
    if (bDeathStarted) return;
    bDeathStarted = true;
    DestroyDelegate = MoveTemp(Func);
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(DeathMontage, 1.0f);
    FOnMontageBlendingOutStarted BlendingOutDelegate;
    BlendingOutDelegate.BindUObject(this, &ASkeletonPawn::OnDeathMontageEnded);
    AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, DeathMontage);
}

void ASkeletonPawn::DoOnSetActive(bool bActive)
{
    if (bActive)
    {
        CurrentAttackTarget = nullptr;
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            AnimInstance->StopAllMontages(0.0f);
            AnimInstance->InitializeAnimation();
        }
    }
    AWeapon* Sword = GetWeapon();
    if (bActive && Sword) Sword->AttachToSkeleton(GetMesh());
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

void ASkeletonPawn::DoUpdatePerceptionOnTeamChange()
{
    if (ASkeletonAIController* S_AIC = GetController<ASkeletonAIController>())
    {
        if (UAIPerceptionSystem* PerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld()))
            PerceptionSys->UpdateListener(*S_AIC->GetAIPerceptionComponent());
        S_AIC->GetAIPerceptionComponent()->ForgetAll();
        S_AIC->GetVisiblePawns().Empty();
        CurrentAttackTarget = nullptr;
    }
}

void ASkeletonPawn::OnAttackMontageEnded(UAnimMontage*, bool) { EndAttack(); }

void ASkeletonPawn::OnDeathMontageEnded(UAnimMontage*, bool) { OnDeathComplete(); }
