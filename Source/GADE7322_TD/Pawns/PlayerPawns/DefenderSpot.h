#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/Actor.h"

#include "DefenderSpot.generated.h"

class APlayerTroop;
class UBoxComponent;

UCLASS(Abstract)
class GADE7322_TD_API ADefenderSpot : public AActor,
                                      public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("DeathEvent"))

public:
    ADefenderSpot();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

public:
    UFUNCTION(BlueprintPure, Category = "Defender")
    bool IsOccupied() const;

    const APlayerTroop* GetCurrentDefender() const { return CurrentDefender; }
    APlayerTroop* GetCurrentDefender() { return CurrentDefender; }

    UFUNCTION(BlueprintCallable, Category = "Defender")
    void PurchaseDefender(const TSubclassOf<APlayerTroop>& DefenderBlueprint);

    UFUNCTION(BlueprintCallable, Category = "Defender")
    void SellDefender();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* BoxCollider = nullptr;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defender", meta = (AllowPrivateAccess = "true"))
    APlayerTroop* CurrentDefender = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    FVector ColliderHalfExtents {50.0f, 50.0f, 50.0f};
};
