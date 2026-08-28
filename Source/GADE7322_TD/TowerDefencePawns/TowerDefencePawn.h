#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "TowerDefencePawn.generated.h"

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
    virtual void StartAttack() { }

    virtual void Attack(ATowerDefencePawn* Other);

    // After finish attack animation or whatever, do any end stuff and if it needs to allow attack again do that
    virtual void EndAttack() { }

    virtual void OnDeath(TFunction<void()>&& Func);

    UFUNCTION(BlueprintCallable)
    virtual void OnDeathComplete(); // If override for animation don't forget to call delegate

    const UHealthComponent* GetHealthComponent() const { return HealthComponent; }
    UHealthComponent* GetHealthComponent() { return HealthComponent; }

    const UDamageComponent* GetDamageComponent() const { return DamageComponent; }
    UDamageComponent* GetDamageComponent() { return DamageComponent; }

    const FName& GetPawnDisplayName() const { return PawnDisplayName; }

    ATowerDefencePawn& SetPawnActive(bool bActive);

    bool IsPawnActive() const { return bIsPawnActive; }

    float GetOccupiedRadius() const { return OccupiedRadius; }

    bool CanAttack() const { return bCanAttack; }

protected:
    // Stuff like toggling mesh, controller, etc.
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

    TFunction<void()> DestroyDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = true))
    bool bIsPawnActive = false;
};
