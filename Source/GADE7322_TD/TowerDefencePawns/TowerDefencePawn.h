#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "TowerDefencePawn.generated.h"

class ATowerDefencePawnAIController;
class UHealthComponent;
class UDamageComponent;
class UAIPerceptionStimuliSourceComponent;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefencePawn : public APawn
{
    GENERATED_BODY()

public:
    ATowerDefencePawn();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // Play like an attack animation, or shoot projectile, etc.
    UFUNCTION(BlueprintCallable)
    virtual void StartAttack() { }

    UFUNCTION(BlueprintCallable)
    virtual void Attack(ATowerDefencePawn* Other);

    // After finish attack animation or whatever, do any end stuff and if it needs to allow attack again do that
    UFUNCTION(BlueprintCallable)
    virtual void EndAttack() { }

    virtual void OnDeath(TFunction<void()>&& Func); // If override for animation don't forget to call delegate

    UFUNCTION(BlueprintCallable)
    virtual void OnDeathComplete();

    const UHealthComponent* GetHealthComponent() const { return HealthComponent; }
    UHealthComponent* GetHealthComponent() { return HealthComponent; }

    const UDamageComponent* GetDamageComponent() const { return DamageComponent; }
    UDamageComponent* GetDamageComponent() { return DamageComponent; }

    const FName& GetPawnDisplayName() const { return PawnDisplayName; }

    ATowerDefencePawn& SetPawnActive(bool bActive);

    bool IsPawnActive() const { return bIsPawnActive; }

    float GetOccupiedRadius() const { return OccupiedRadius; }

    bool CanAttack() const { return bCanAttack; }

    bool UseAIController() const { return bUseAIController; }

    TSubclassOf<ATowerDefencePawnAIController> GetTDAIControllerClass() const { return TDAIControllerClass; }

protected:
    // Stuff like toggling mesh, etc.
    virtual void DoOnSetActive(bool bActive) { }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UDamageComponent* DamageComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name", meta = (AllowPrivateAccess = true))
    FName PawnDisplayName = TEXT("TowerDefencePawn");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Size",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float OccupiedRadius = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = true))
    bool bCanAttack = true;

    UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (DisplayName = "TD AI Controller Class"))
    TSubclassOf<ATowerDefencePawnAIController> TDAIControllerClass;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    bool bUseAIController = false;

    TFunction<void()> DestroyDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = true))
    bool bIsPawnActive = false;
};
