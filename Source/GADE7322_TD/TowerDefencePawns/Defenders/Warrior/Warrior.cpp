#include "TowerDefencePawns/Defenders/Warrior/Warrior.h"

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
    // if (ASkeletonAIController* AIController = Cast<ASkeletonAIController>(GetController()))
    //     AIController->SetControllerActive(bActive);
}

const USkeletalMesh* AWarrior::GetMaleMesh() const { return MaleMesh; }

USkeletalMesh* AWarrior::GetMaleMesh() { return MaleMesh; }

const USkeletalMesh* AWarrior::GetFemaleMesh() const { return FemaleMesh; }

USkeletalMesh* AWarrior::GetFemaleMesh() { return FemaleMesh; }

const USkeletalMeshComponent* AWarrior::GetMesh() const { return SkeletalMesh; }

USkeletalMeshComponent* AWarrior::GetMesh() { return SkeletalMesh; }

bool AWarrior::IsFemale() const { return bIsFemale; }
