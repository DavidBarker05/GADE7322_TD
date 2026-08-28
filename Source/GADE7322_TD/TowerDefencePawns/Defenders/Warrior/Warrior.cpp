// ReSharper disable CppParameterMayBeConst
#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

AWarrior::AWarrior()
{
    PawnDisplayName = TEXT("Warrior");
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
}

void AWarrior::BeginPlay()
{
    Super::BeginPlay();
    // Gender randomisation wasn't working in constructor :(
    bIsFemale = FMath::RandBool();
    SkeletalMesh->SetSkeletalMesh(bIsFemale ? FemaleMesh : MaleMesh);
    SkeletalMesh->SetAnimClass(bIsFemale ? FemaleAnimationBlueprint : MaleAnimationBlueprint);
}

void AWarrior::StartAttack()
{
    bCanAttack = false;
    // Play animation
}

void AWarrior::EndAttack() { bCanAttack = true; }

void AWarrior::DoOnSetActive(bool bActive)
{
    if (ATowerDefencePawnAIController* AIController = Cast<ATowerDefencePawnAIController>(GetController()))
        AIController->SetControllerActive(bActive);
}
