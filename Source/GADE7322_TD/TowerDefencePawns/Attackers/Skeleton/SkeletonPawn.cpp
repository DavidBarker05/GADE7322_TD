#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"

#include "DamageComponent.h"
#include "SkeletonAIController.h"

ASkeletonPawn::ASkeletonPawn()
{
    PawnDisplayName = TEXT("Skeleton");
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
}

void ASkeletonPawn::BeginPlay()
{
    Super::BeginPlay();
    SkeletalMesh->SetSkeletalMesh(PossibleMeshes[FMath::RandRange(0, PossibleMeshes.Num() - 1)]);
}

void ASkeletonPawn::StartAttack()
{
    bCanAttack = false;
    // Play animation
}

void ASkeletonPawn::EndAttack() { bCanAttack = true; }

void ASkeletonPawn::DoOnSetActive(bool bActive)
{
    if (ASkeletonAIController* AIController = Cast<ASkeletonAIController>(GetController()))
        AIController->SetControllerActive(bActive);
}
