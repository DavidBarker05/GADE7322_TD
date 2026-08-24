#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "ProceduralTerrainGen.generated.h"

UCLASS()
class GADE7322_TD_API AProceduralTerrainGen : public AActor
{
    GENERATED_BODY()

public:
    AProceduralTerrainGen();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
