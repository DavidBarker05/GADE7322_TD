#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

#include "Defender.generated.h"

class ADefenderSpot;
class UBoxComponent;
class UNiagaraComponent;

UCLASS(Abstract)
class GADE7322_TD_API ADefender : public ATowerDefencePawn
{
    GENERATED_BODY()

public:
    ADefender();

    virtual void BeginPlay() override;

    void ShowRadiusDisplay();

    void HideRadiusDisplay();

    virtual float GetDetectionRadius() const;
    virtual float GetPrimaryRadius() const { return 0.0f; }
    virtual float GetSecondaryRadius() const { return 0.0f; }

    int32 GetCost() const { return Cost; }

    int32 GetSellPrice() const { return SellPrice; }

    ADefender& SetSpawnPoint(ADefenderSpot* Point)
    {
        SpawnPoint = Point;
        return *this;
    }

    const ADefenderSpot* GetSpawnPoint() const { return SpawnPoint; }
    ADefenderSpot* GetSpawnPoint() { return SpawnPoint; }

    const UNiagaraComponent* GetHealEffect() const { return HealEffect; }
    UNiagaraComponent* GetHealEffect() { return HealEffect; }

    const UStaticMeshComponent* GetDefenderRadiusDisplay() const { return DefenderRadiusDisplay; }
    UStaticMeshComponent* GetDefenderRadiusDisplay() { return DefenderRadiusDisplay; }

    const UMaterialInstanceDynamic* GetDefenderRadiusMaterial() const { return DefenderRadiusMaterial; }
    UMaterialInstanceDynamic* GetDefenderRadiusMaterial() { return DefenderRadiusMaterial; }

protected:
    virtual void DoOnSetActive(bool bActive) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Cost = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 SellPrice = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Defender", meta = (AllowPrivateAccess = true))
    FLinearColor DetectionRadiusColour = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Defender", meta = (AllowPrivateAccess = true))
    FLinearColor PrimaryRadiusColour;

    UPROPERTY(EditDefaultsOnly, Category = "Defender", meta = (AllowPrivateAccess = true))
    FLinearColor SecondaryRadiusColour;

private:
    UPROPERTY(BlueprintReadWrite, Category = "Defender", meta = (AllowPrivateAccess = true))
    ADefenderSpot* SpawnPoint = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UBoxComponent* BoxCollider = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* HealEffect;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* DefenderRadiusDisplay;

    UPROPERTY(BlueprintReadOnly, Category = "Defender", meta = (AllowPrivateAccess = true))
    UMaterialInstanceDynamic* DefenderRadiusMaterial;
};
