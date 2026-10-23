#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/TowerDefencePawn.h"

#include "Defender.generated.h"

class ADefenderSpot;
class UBoxComponent;
class UDefenderUpgradeComponent;
class UNiagaraComponent;

UCLASS(Abstract)
class GADE7322_TD_API ADefender : public ATowerDefencePawn
{
    GENERATED_BODY()

public:
    ADefender();

    virtual void BeginPlay() override;

    virtual void PostInitializeComponents() override;

    void ShowRadiusDisplay();

    void HideRadiusDisplay();

    virtual float GetDetectionRadius() const;
    virtual float GetPrimaryRadius() const { return 0.0f; }
    virtual float GetSecondaryRadius() const { return 0.0f; }

    // Returns the name of the defender for the specified level if the defender class and level are valid
    static TOptional<FName> GetDefenderName(TSubclassOf<ADefender> DefenderClass, int32 Level);

    // Returns the cost of the defender for the specified level if the defender class and level are valid
    static TOptional<int32> GetDefenderCost(TSubclassOf<ADefender> DefenderClass, int32 Level);

    // Returns the sell price for the defender at its current level
    UFUNCTION(BlueprintPure)
    int32 GetSellPrice() const;

    ADefender& SetSpawnPoint(ADefenderSpot* Point)
    {
        SpawnPoint = Point;
        return *this;
    }

    const ADefenderSpot* GetSpawnPoint() const { return SpawnPoint; }
    ADefenderSpot* GetSpawnPoint() { return SpawnPoint; }

    const UBoxComponent* GetBoxCollider() const { return BoxCollider; }
    UBoxComponent* GetBoxCollider() { return BoxCollider; }

    const UNiagaraComponent* GetHealEffect() const { return HealEffect; }
    UNiagaraComponent* GetHealEffect() { return HealEffect; }

    const UStaticMeshComponent* GetDefenderRadiusDisplay() const { return DefenderRadiusDisplay; }
    UStaticMeshComponent* GetDefenderRadiusDisplay() { return DefenderRadiusDisplay; }

    const UMaterialInstanceDynamic* GetDefenderRadiusMaterial() const { return DefenderRadiusMaterial; }
    UMaterialInstanceDynamic* GetDefenderRadiusMaterial() { return DefenderRadiusMaterial; }

    const UDefenderUpgradeComponent* GetUpgradeComponent() const { return UpgradeComponent; }
    UDefenderUpgradeComponent* GetUpgradeComponent() { return UpgradeComponent; }

protected:
    virtual void DoOnSetActive(bool bActive) override;

    virtual void DoOnLevelChanged(int32 Level) { }

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
    UFUNCTION()
    virtual void LevelChanged(int32 Level);

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UDefenderUpgradeComponent* UpgradeComponent;
};
