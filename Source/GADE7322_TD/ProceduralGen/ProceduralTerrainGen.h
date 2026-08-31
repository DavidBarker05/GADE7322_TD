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

    // Builds the terrain mesh, flattening a corridor around each generated path. Also bakes a
    // path/terrain blend mask into vertex colors so a single material can render both looks - see
    // the R channel comment on VertexColors in the .cpp for how to use it.
    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void GenerateTerrain() const;

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

    // Radius of the terrain from the tower (0,0) out to the edge spawn points
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 500.0))
    float TerrainRadius = 10000.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 3))
    int32 NumPaths = 3;

    // How many straight segments each path is subdivided into before noise is applied. These are
    // coarse control points for the curve below, not the final path shape.
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 4))
    int32 PathSegments = 24;

    // Max perpendicular offset (world units) the path can wander from the straight line to the tower
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 0.0))
    float PathWanderAmount = 1100.0f;

    // How rapidly the wander offset changes along the path. Lower = broader, slower sweeping curves;
    // higher = the path reverses direction between control points more often (wigglier, more jagged)
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 0.01))
    float PathWanderFrequency = 0.2f;

    // How many smooth points to generate between each pair of control points. 1 = no smoothing (the
    // old sharp-cornered polyline), higher = smoother curve but more distance/mesh checks per path.
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 1))
    int32 SplineSubdivisions = 8;

    // Half-width of the flat walkable strip each path carves through the terrain
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 50.0))
    float PathWidth = 200.0f;

    // Minimum angular gap (degrees) enforced between neighbouring path entry points
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 0.0, ClampMax = 180.0))
    float MinEntryAngleSeparation = 25.0f;

    // Minimum gap enforced between two different paths' corridor edges (not centrelines). Paths
    // built after the first steer away from every already-built path if they'd end up closer than
    // this, so they generally stop crossing/overlapping each other in the middle of the map.
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 0.0))
    float MinPathSeparation = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation")
    bool bRandomSeedEachGame = true;

    // Generate using a specific seed
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (EditCondition = "!bRandomSeedEachGame"))
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug")
    bool bDrawDebugPaths = true;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug", meta = (EditCondition = "bDrawDebugPaths"))
    float DebugDrawDuration = 30.0f;

    // World units per grid cell (smaller = denser mesh/more triangles)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 50.0))
    float CellSize = 200.0f;

    // World units of vertical relief added by noise, before path flattening
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0))
    float HeightAmplitude = 600.0f;

    // Larger = more gradual hills (lower frequency noise)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 100.0))
    float NoiseWavelength = 2500.0f;

    // Number of fractal noise layers combined together (higher = rougher/more detailed terrain)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 1, ClampMax = 6))
    int32 NoiseOctaves = 4;

    // Extra distance beyond each path's Width that stays completely flat (same PathHeight as the
    // corridor itself) before the height blend below even starts. This is the "won't clip a tower"
    // guarantee - keep it at least as large as DefenderSpotOffset so every placement spot lands on
    // guaranteed-flat ground, not partway up the noise ramp.
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0))
    float PathFlatZoneWidth = 250.0f;

    // Distance beyond PathFlatZoneWidth over which terrain HEIGHT ramps from flat up to full noise
    // height - this is the "gradually get bumpier farther out" transition. Kept separate from
    // PathTextureBlendWidth below: this can be wide for a gentle ramp without making the visible path
    // itself look wider than the actual walkable corridor.
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0))
    float PathHeightBlendWidth = 400.0f;

    // Extra distance beyond each path's Width over which the vertex-colour mask (and so the material's
    // path/terrain blend) transitions from path look to terrain look. Independent of
    // PathHeightBlendWidth so the texture edge can be tighter (or wider) than the height ramp.
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0))
    float PathTextureBlendWidth = 150.0f;

    // World Z of the flat ground along paths
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh")
    float PathHeight = 0.0f;

    // Should blend between a path look and a terrain look using the vertex-colour mask GenerateTerrain()
    // bakes in (VertexColor.R: 0 = path, 1 = terrain). See the .cpp for the material graph setup.
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh")
    TObjectPtr<UMaterialInterface> TerrainMaterial;

    UPROPERTY(EditAnywhere, Category = "Defender Spots")
    TSubclassOf<ADefenderSpot> DefenderSpotClass;

    // Arc-length distance walked along a path between candidate spot positions
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 50.0))
    float DefenderSpotSpacing = 500.0f;

    // Extra clearance beyond the path's flattened corridor edge before placing a spot
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0))
    float DefenderSpotOffset = 150.0f;

    // Minimum distance enforced between two spots so their colliders never overlap
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0))
    float DefenderSpotMinSeparation = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug")
    bool bDrawDebugDefenderSpots = true;

private:
    // Fractal (multi-octave) Perlin height sample at a world X/Y, before any path flattening
    float SampleNoiseHeight(const FVector2D& WorldXY) const;

    // Signed distance from Point to the nearest path corridor's edge
    float DistanceToNearestPathEdge(const FVector2D& Point) const;

    // Height (blended over PathHeightBlendWidth) and vertex-colour/texture blend alpha (blended over
    // PathTextureBlendWidth, independently) at a world X/Y. Computed together since both start from
    // the same DistanceToNearestPathEdge call - GetTerrainHeight() only needs the height half of
    // this, but GenerateTerrain() needs both per vertex and shouldn't pay for that distance check twice.
    void SampleTerrainPoint(const FVector2D& WorldXY, float& OutHeight, float& OutTextureBlendAlpha) const;

    // Picks NumPaths angles around the circle, evenly spaced then jittered, respecting MinEntryAngleSeparation
    TArray<float> GenerateEntryAngles(const FRandomStream& Stream) const;

    FTerrainPath BuildPath(float EntryAngleDegrees, const FRandomStream& Stream) const;

    // The coarse, noisy, straight-segment-connected points BuildPath used to return directly -
    // still the source of the path's overall shape/wander, just not the final rendered curve.
    TArray<FVector> BuildPathControlPoints(float EntryAngleDegrees, const FRandomStream& Stream) const;

    // How far, and in which direction, a candidate point on the path currently being built should
    // be nudged to keep MinPathSeparation from every already-built path in Paths. Zero vector if
    // it's already far enough from all of them.
    FVector ComputePathSeparationOffset(const FVector& Point) const;

    // Runs a Catmull-Rom spline through the coarse control points, replacing the sharp corners
    // where the path used to visibly bounce off its wander target with smooth curves.
    TArray<FVector> SmoothPathControlPoints(const TArray<FVector>& ControlPoints) const;

    void DrawDebugForPaths() const;

    // Spawns a spot at CandidateLocation if it clears every path corridor and every previously placed spot
    void TrySpawnDefenderSpot(const FVector& CandidateLocation, TArray<FVector>& PlacedLocations);

    void DrawDebugForDefenderSpots() const;

    UPROPERTY(BlueprintReadOnly, Category = "Terrain Generation", meta = (AllowPrivateAccess = true))
    TArray<FTerrainPath> Paths;

    UPROPERTY(BlueprintReadOnly, Category = "Defender Spots", meta = (AllowPrivateAccess = true))
    TArray<ADefenderSpot*> DefenderSpots;
};
