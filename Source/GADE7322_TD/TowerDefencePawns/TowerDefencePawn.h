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

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

public:
    // Play like an attack animation, or shoot projectile, etc.
    virtual void StartAttack() { }

    virtual void Attack(ATowerDefencePawn* Other);

    // After finish attack animation or whatever, do any end stuff and if need to allow attack again do that
    virtual void EndAttack() { }

    virtual void OnDeath(TFunction<void()>&& Func);

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnDeathComplete(); // If override for animation don't forget to call delegate

public:
    const UHealthComponent* GetHealthComponent() const;
    UHealthComponent* GetHealthComponent();

    const UDamageComponent* GetDamageComponent() const;
    UDamageComponent* GetDamageComponent();

    const FName& GetPawnDisplayName() const;

    ATowerDefencePawn& SetPawnActive(bool bActive);

    bool IsPawnActive() const { return bIsPawnActive; }

    float GetOccupiedRadius() const { return OccupiedRadius; }

    bool CanAttack() const { return bCanAttack; }

protected:
    // Stuff like toggling mesh, controller, etc.
    virtual void DoOnSetActive(bool bActive) { }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UDamageComponent* DamageComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name", meta = (AllowPrivateAccess = "true"))
    FName PawnDisplayName = TEXT("TowerDefencePawn");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Size",
              meta = (AllowPrivateAccess = "true", Units = "Centimeters", ClampMin = "0.0", UIMin = "0.0"))
    float OccupiedRadius = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = "true"))
    bool bCanAttack = true;

    TFunction<void()> DestroyDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = "true"))
    bool bIsPawnActive = false;
};
