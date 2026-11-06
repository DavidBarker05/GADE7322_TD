#pragma once

#include "TDCore.h"

#include "GameFramework/Actor.h"
#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemCollapse.h"

#include "GolemPart.generated.h"

class UHitFlashComponent;

UCLASS(Abstract)
class AGolemPart : public AActor
{
    GENERATED_BODY()

public:
    AGolemPart();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Golem|Part")
    virtual void EnableMesh();

    UFUNCTION(BlueprintCallable, Category = "Golem|Part")
    virtual void DisableMesh();

    const UHitFlashComponent* GetHitFlashComponent() const { return HitFlashComponent; }
    UHitFlashComponent* GetHitFlashComponent() { return HitFlashComponent; }

    const TArray<UStaticMeshComponent*>& GetRocks() const { return Rocks; }
    TArray<UStaticMeshComponent*>& GetRocks() { return Rocks; }

    void StartCollapse(float LinearImpulse, float SpinDegrees);

    void StopCollapse();

private:
    void SetRocksActive(bool bActive);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Golem|Part|Components",
              meta = (AllowPrivateAccess = true))
    UHitFlashComponent* HitFlashComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Golem|Part|Components", meta = (AllowPrivateAccess = true))
    TArray<UStaticMeshComponent*> Rocks;

    FGolemCollapse Collapse;
};
