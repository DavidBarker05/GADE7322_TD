#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"

#include "ProceduralTerrainGen.generated.h"

USTRUCT(BlueprintType)
struct FTerrainPath
{
    GENERATED_BODY()

    // Ordered points from the edge spawn point to the tower, in world space
    UPROPERTY(BlueprintReadOnly, Category = "Terrain Path")
    TArray<FVector> Points;

    // Half-width of the walkable strip either side of the path centreline
    UPROPERTY(BlueprintReadOnly, Category = "Terrain Path")
    float Width = 0.0f;
};

UCLASS()
class GADE7322_TD_API AProceduralTerrainGen : public AActor
{
    GENERATED_BODY()

public:
    AProceduralTerrainGen();

protected:
    virtual void BeginPlay() override;

public:
    // (Re)generates the enemy pathways. Safe to call again to reroll layout.
    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void GeneratePaths();

    UFUNCTION(BlueprintPure, Category = "Terrain Generation")
    const TArray<FTerrainPath>& GetPaths() const { return Paths; }

protected:
    // Radius of the terrain from the tower (0,0) out to the edge spawn points
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "500.0"))
    float TerrainRadius = 4000.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "3"))
    int32 NumPaths = 3;

    // How many straight segments each path is subdivided into before noise is applied
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "4"))
    int32 PathSegments = 24;

    // Max perpendicular offset (world units) the path can wander from the straight line to the tower
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "0.0"))
    float PathWanderAmount = 900.0f;

    // How rapidly the wander offset changes along the path. Higher = wigglier
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "0.01"))
    float PathWanderFrequency = 0.6f;

    // Half-width of the flat walkable strip each path carves through the terrain
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "50.0"))
    float PathWidth = 250.0f;

    // Minimum angular gap (degrees) enforced between neighbouring path entry points
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float MinEntryAngleSeparation = 25.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation")
    bool bRandomSeedEachGame = true;

    // Generate using a specific seed
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (EditCondition = "!bRandomSeedEachGame"))
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug")
    bool bDrawDebugPaths = true;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug", meta = (EditCondition = "bDrawDebugPaths"))
    float DebugDrawDuration = 30.0f;

private:
    // Picks NumPaths angles around the circle, evenly spaced then jittered, respecting MinEntryAngleSeparation
    TArray<float> GenerateEntryAngles(FRandomStream& Stream) const;

    FTerrainPath BuildPath(float EntryAngleDegrees, FRandomStream& Stream) const;

    void DrawDebugForPaths() const;

private:
    UPROPERTY(BlueprintReadOnly, Category = "Terrain Generation", meta = (AllowPrivateAccess = "true"))
    TArray<FTerrainPath> Paths;
};
