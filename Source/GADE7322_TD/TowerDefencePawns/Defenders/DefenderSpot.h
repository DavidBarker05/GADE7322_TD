#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/Actor.h"

#include "DefenderSpot.generated.h"

class ADefender;
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

    UFUNCTION(BlueprintPure, Category = "Defender")
    bool IsOccupied() const;

    const ADefender* GetCurrentDefender() const { return CurrentDefender; }
    ADefender* GetCurrentDefender() { return CurrentDefender; }

    UFUNCTION(BlueprintCallable, Category = "Defender")
    void PurchaseDefender(const TSubclassOf<ADefender>& DefenderBlueprint);

    UFUNCTION(BlueprintCallable, Category = "Defender")
    void SellDefender();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UBoxComponent* BoxCollider = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Defender", meta = (AllowPrivateAccess = true))
    ADefender* CurrentDefender = nullptr;
};
