#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "TowerDefensePawn.generated.h"

class UHealthComponent;
class UDamageComponent;

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefensePawn : public APawn
{
    GENERATED_BODY()

public:
    ATowerDefensePawn();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

public:
    virtual void Attack(ATowerDefensePawn* Other);

    virtual void OnDeath(TFunction<void()>&& Func);

public:
    UFUNCTION(BlueprintCallable)
    void OnDeathComplete(); // If override for animation don't forget to set delegate

public:
    const UHealthComponent* GetHealthComponent() const;
    UHealthComponent* GetHealthComponent();

    const UDamageComponent* GetDamageComponent() const;
    UDamageComponent* GetDamageComponent();

    const FName& GetPawnDisplayName() const;

protected:
    // Stuff like hide mesh, etc.
    virtual void DoOnDeathComplete() { }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UDamageComponent* DamageComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name", meta = (AllowPrivateAccess = "true"))
    FName PawnDisplayName = TEXT("TowerDefensePawn");

    TFunction<void()> DestroyDelegate;
};
