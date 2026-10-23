#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "DefenderUpgradeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, Level);

USTRUCT(BlueprintType)
struct FDefenderLevel
{
    GENERATED_BODY()

    FName GetName() const { return UpgradeName; }
    int32 GetCost() const { return Cost; }
    int32 GetSellPrice() const { return SellPrice; }
    int32 GetHealth() const { return Health; }
    int32 GetDamage() const { return Damage; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades|Tower Defence Pawn",
              meta = (AllowPrivateAccess = true))
    FName UpgradeName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades|Cost",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Cost = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades|Cost",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 SellPrice = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades|Health",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Health = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades|Damage",
              meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0))
    int32 Damage = 0;
};

UCLASS(ClassGroup = (Defender), meta = (BlueprintSpawnableComponent))
class UDefenderUpgradeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    FDefenderLevel GetBaseLevel() const { return BaseLevel; }
    FDefenderLevel GetFirstUpgrade() const { return FirstUpgrade; }
    FDefenderLevel GetSecondUpgrade() const { return SecondUpgrade; }

    UFUNCTION(BlueprintPure)
    FDefenderLevel GetCurrent() const;

    int32 GetCurrentLevel() const { return CurrentLevel; }
    UFUNCTION(BlueprintCallable)
    void SetLevel(int32 Level);
    UFUNCTION(BlueprintCallable)
    void Upgrade();

    UPROPERTY(BlueprintAssignable)
    FOnLevelChanged OnLevelChanged;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Upgrades, meta = (AllowPrivateAccess = true))
    FDefenderLevel BaseLevel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Upgrades, meta = (AllowPrivateAccess = true))
    FDefenderLevel FirstUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Upgrades, meta = (AllowPrivateAccess = true))
    FDefenderLevel SecondUpgrade;

    UPROPERTY(BlueprintReadOnly, Category = Upgrades, meta = (AllowPrivateAccess = true))
    int32 CurrentLevel = 0;
};
