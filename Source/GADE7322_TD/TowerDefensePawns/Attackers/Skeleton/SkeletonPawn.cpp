#include "TowerDefensePawns/Attackers/Skeleton/SkeletonPawn.h"

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

void ASkeletonPawn::DoOnSetActive()
{
    if (ASkeletonAIController* AIController = Cast<ASkeletonAIController>(GetController()))
        AIController->SetControllerActive(true);
}

void ASkeletonPawn::DoOnDeathComplete()
{
    if (ASkeletonAIController* AIController = Cast<ASkeletonAIController>(GetController()))
        AIController->SetControllerActive(false);
}
