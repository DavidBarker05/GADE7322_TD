#pragma once

#include "CoreMinimal.h"

#include "AI/Navigation/NavigationTypes.h"
#include "TowerDefencePawns/AI/TowerDefencePawnAIController.h"

#include "DefenderAIController.generated.h"

class ADefender;

USTRUCT(BlueprintType, meta = (DisplayName = "Defender AI Controller Level"))
struct FDefenderAIControllerLevel
{
    GENERATED_BODY()

    float GetDetectionRadius() const { return DetectionRadius; }

    bool DetectEnemies() const { return bDetectEnemies; }
    bool DetectNeutrals() const { return bDetectNeutrals; }
    bool DetectFriendlies() const { return bDetectFriendlies; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DetectionRadius = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true))
    bool bDetectEnemies = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true))
    bool bDetectNeutrals = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true))
    bool bDetectFriendlies = false;
};

UCLASS(Abstract)
class GADE7322_TD_API ADefenderAIController : public ATowerDefencePawnAIController
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    ADefender* GetDefender() const;

    FDefenderAIControllerLevel GetControllerBaseLevel() const { return ControllerBaseLevel; }
    FDefenderAIControllerLevel GetControllerFirstUpgrade() const { return ControllerFirstUpgrade; }
    FDefenderAIControllerLevel GetControllerSecondUpgrade() const { return ControllerSecondUpgrade; }

    UFUNCTION(BlueprintPure)
    FDefenderAIControllerLevel GetControllerCurrentLevel() const;

    virtual void UpdateControllerLevel();

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true))
    FDefenderAIControllerLevel ControllerBaseLevel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true))
    FDefenderAIControllerLevel ControllerFirstUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defender|Controller|Upgrades",
              meta = (AllowPrivateAccess = true))
    FDefenderAIControllerLevel ControllerSecondUpgrade;
};
