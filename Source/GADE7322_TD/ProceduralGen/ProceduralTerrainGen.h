#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"

#include "ProceduralTerrainGen.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;
class ADefenderSpot;

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
    // (Re)generates the enemy pathways
    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void GeneratePaths();

    UFUNCTION(BlueprintPure, Category = "Terrain Generation")
    const TArray<FTerrainPath>& GetPaths() const { return Paths; }

    // Builds the terrain mesh, flattening a corridor around each generated path
    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void GenerateTerrain();

    // Samples the terrain height (Z) at a given world X/Y, using the same heightfield the mesh was built from
    UFUNCTION(BlueprintPure, Category = "Terrain Generation")
    float GetTerrainHeight(const FVector2D& WorldXY) const;

    // Spawns defender placement spots along both sides of every path, clear of the walkable corridors
    UFUNCTION(BlueprintCallable, Category = "Defender Spots")
    void GenerateDefenderSpots();

    UFUNCTION(BlueprintPure, Category = "Defender Spots")
    const TArray<ADefenderSpot*>& GetDefenderSpots() const { return DefenderSpots; }

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UProceduralMeshComponent> TerrainMesh;

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

    // How rapidly the wander offset changes along the path (higher = wigglier)
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

protected:
    // World units per grid cell (smaller = denser mesh/more triangles)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = "50.0"))
    float CellSize = 200.0f;

    // World units of vertical relief added by noise, before path flattening
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = "0.0"))
    float HeightAmplitude = 600.0f;

    // Larger = more gradual hills (lower frequency noise)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = "100.0"))
    float NoiseWavelength = 2500.0f;

    // Number of fractal noise layers combined together (higher = rougher/more detailed terrain)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = "1", ClampMax = "6"))
    int32 NoiseOctaves = 4;

    // Extra distance beyond each path's Width over which terrain blends from flat back up to full noise height
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = "0.0"))
    float PathBlendWidth = 400.0f;

    // World Z of the flat ground along paths
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh")
    float PathHeight = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Mesh")
    TObjectPtr<UMaterialInterface> TerrainMaterial;

protected:
    UPROPERTY(EditAnywhere, Category = "Defender Spots")
    TSubclassOf<ADefenderSpot> DefenderSpotClass;

    // Arc-length distance walked along a path between candidate spot positions
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = "50.0"))
    float DefenderSpotSpacing = 500.0f;

    // Extra clearance beyond the path's flattened corridor edge before placing a spot
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = "0.0"))
    float DefenderSpotOffset = 150.0f;

    // Minimum distance enforced between two spots so their colliders never overlap
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = "0.0"))
    float DefenderSpotMinSeparation = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug")
    bool bDrawDebugDefenderSpots = true;

private:
    // Fractal (multi-octave) Perlin height sample at a world X/Y, before any path flattening
    float SampleNoiseHeight(const FVector2D& WorldXY) const;

    // Signed distance from Point to the nearest path corridor's edge
    float DistanceToNearestPathEdge(const FVector2D& Point) const;

private:
    // Picks NumPaths angles around the circle, evenly spaced then jittered, respecting MinEntryAngleSeparation
    TArray<float> GenerateEntryAngles(FRandomStream& Stream) const;

    FTerrainPath BuildPath(float EntryAngleDegrees, FRandomStream& Stream) const;

    void DrawDebugForPaths() const;

    // Spawns a spot at CandidateLocation if it clears every path corridor and every previously placed spot
    void TrySpawnDefenderSpot(const FVector& CandidateLocation, TArray<FVector>& PlacedLocations);

    void DrawDebugForDefenderSpots() const;

private:
    UPROPERTY(BlueprintReadOnly, Category = "Terrain Generation", meta = (AllowPrivateAccess = "true"))
    TArray<FTerrainPath> Paths;

    UPROPERTY(BlueprintReadOnly, Category = "Defender Spots", meta = (AllowPrivateAccess = "true"))
    TArray<ADefenderSpot*> DefenderSpots;
};
