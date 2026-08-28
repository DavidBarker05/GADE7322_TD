// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Attackers/Skeleton/SkeletonPawn.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

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
    if (ATowerDefencePawnAIController* AIController = Cast<ATowerDefencePawnAIController>(GetController()))
        AIController->SetControllerActive(bActive);
}
