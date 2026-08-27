#pragma once

#include "CoreMinimal.h"

#include "TowerDefensePawns/Defenders/Defender.h"

#include "Warrior.generated.h"

class USkeletalMesh;
class UStaticMeshComponent;

UCLASS(Abstract)
class GADE7322_TD_API AWarrior : public ADefender
{
    GENERATED_BODY()

public:
    AWarrior();

    virtual void BeginPlay() override;

public:
    const USkeletalMesh* GetMaleMesh() const;
    USkeletalMesh* GetMaleMesh();

    const USkeletalMesh* GetFemaleMesh() const;
    USkeletalMesh* GetFemaleMesh();

    const USkeletalMeshComponent* GetMesh() const;
    USkeletalMeshComponent* GetMesh();

    bool IsFemale() const;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    USkeletalMesh* MaleMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    USkeletalMesh* FemaleMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* SkeletalMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gender", meta = (AllowPrivateAccess = "true"))
    bool bIsFemale;
};
