#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"

#include "ProceduralTerrainGen.generated.h"

class UProceduralMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;
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

// Grass
USTRUCT(BlueprintType)
struct FProceduralGrass
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Grass")
    UStaticMesh* GrassMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Grass", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MinScale = 0.4f;

    UPROPERTY(EditDefaultsOnly, Category = "Grass", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MaxScale = 0.6f;
};

// Foliage like mushrooms and stuff that would look bad overlapping
USTRUCT(BlueprintType)
struct FProceduralFoliage
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Foliage")
    UStaticMesh* FoliageMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Foliage", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MinScale = 0.85f;

    UPROPERTY(EditDefaultsOnly, Category = "Foliage", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MaxScale = 1.15f;

    UPROPERTY(EditDefaultsOnly, Category = "Foliage", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float OccupiedRadius = 25.0f;

    // How common the foliage is, less common foliage should appear less often
    UPROPERTY(EditDefaultsOnly, Category = "Foliage", meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
    float SpawnChance = 1.0f;
};

// Trees, similar trees should want to clump together but can be a variety nearby
// shouldn't spawn as much of these because they have collision
USTRUCT(BlueprintType)
struct FProceduralTree
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    UStaticMesh* TreeMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Tree", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MinScale = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Tree", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MaxScale = 0.8f;

    UPROPERTY(EditDefaultsOnly, Category = "Tree", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float OccupiedRadius = 100.0f;
};

// These will have collision and will be short enough that they will definitely affect the navmesh
// these should not be close to the path at all so that they don't ruin the navigation
USTRUCT(BlueprintType)
struct FProceduralRock
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Rock")
    UStaticMesh* RockMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Rock", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MinScale = 0.8f;

    UPROPERTY(EditDefaultsOnly, Category = "Rock", meta = (ClampMin = 0.0, UIMin = 0.0))
    float MaxScale = 1.2f;

    UPROPERTY(EditDefaultsOnly, Category = "Rock", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float OccupiedRadius = 50.0f;
};

// How a nature layer thins out near paths, the tower plaza and defender spots. Distances are measured from the
// edge of the path corridor/spot footprint, whichever is closest
USTRUCT(BlueprintType)
struct FNatureClearance
{
    GENERATED_BODY()

    // Nothing spawns closer than this
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float MinDistance = 50.0f;

    // Beyond MinDistance, how far it takes for the density to climb from NearDensity back up to full
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float FalloffDistance = 400.0f;

    // Density multiplier right at MinDistance (1 = no thinning near paths/spots, 0 = nothing until the falloff ends)
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
    float NearDensity = 0.5f;
};

// Shared scatter settings for one kind of nature (grass, foliage, trees, rocks). The scatter walks a jittered grid
// over the whole terrain and rejects candidates against these in order, cheapest check first
USTRUCT(BlueprintType)
struct FNatureLayer
{
    GENERATED_BODY()

    // Distance between candidate points (each one is jittered). Smaller = more instances
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 25.0, UIMin = 25.0, Units = "Centimeters"))
    float Spacing = 200.0f;

    // Chance that a candidate which passes every other check actually spawns
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
    float Density = 0.5f;

    // Size of the noise blobs used to clump this layer into patches. 0 = no patches, spawns everywhere
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float PatchWavelength = 0.0f;

    // Noise value (0-1) a patch starts at. Higher = fewer and smaller patches
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
    float PatchThreshold = 0.6f;

    UPROPERTY(EditAnywhere, Category = "Nature")
    FNatureClearance Clearance;

    // Steepest ground this can spawn on. 90 = no limit
    UPROPERTY(EditAnywhere, Category = "Nature",
              meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 90.0, UIMax = 90.0, Units = "Degrees"))
    float MaxSlope = 90.0f;

    // Added to the sampled terrain height. Negative sinks the mesh a little, hiding gaps on slopes
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (Units = "Centimeters"))
    float VerticalOffset = 0.0f;

    // Instances further than this from the camera aren't drawn (none of the nature meshes have LODs). 0 = never culled
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float CullDistance = 0.0f;

    // Hard cap on how many of this layer can spawn
    UPROPERTY(EditAnywhere, Category = "Nature", meta = (ClampMin = 0, UIMin = 0))
    int32 MaxInstances = 5000;
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
    // path/terrain blend mask into vertex colours so a single material can render both looks
    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void GenerateTerrain() const;

    // Samples the terrain height (Z) at a given world X/Y, using the same heightfield the mesh was built from
    UFUNCTION(BlueprintPure, Category = "Terrain Generation")
    float GetTerrainHeight(const FVector2D& WorldXY) const;

    // (Re)computes defender spot locations along both sides of every path and (re)spawns them there
    UFUNCTION(BlueprintCallable, Category = "Defender Spots")
    void GenerateDefenderSpots();

    // Bake the procedural mesh into a more optimised static mesh
    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void BakeMesh();

    UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
    void RebuildNavMesh() const;

    // (Re)scatters grass, foliage, trees and rocks over the terrain
    UFUNCTION(BlueprintCallable, Category = "Nature")
    void GenerateNature();

    UFUNCTION(BlueprintPure, Category = "Defender Spots")
    const TArray<ADefenderSpot*>& GetDefenderSpots() const { return DefenderSpots; }

    UFUNCTION(BlueprintPure, Category = "Terrain Generation")
    float GetTerrainHalfExtent() const
    {
        return TerrainRadius + PathWanderAmount + PathWidth + PathFlatZoneWidth +
               FMath::Max(PathHeightBlendWidth, PathTextureBlendWidth);
    }

protected:
    UPROPERTY()
    UProceduralMeshComponent* TerrainMesh;

    // Final baked version of UProceduralMeshComponent converted to a UStaticMeshComponent
    // which is better for performance
    UPROPERTY()
    UStaticMeshComponent* BakedTerrainMesh;

    // Radius of the terrain from the tower (0,0) out to the edge spawn points
    UPROPERTY(EditAnywhere, Category = "Terrain Generation",
              meta = (ClampMin = 500.0, UIMin = 500.0, Units = "Centimeters"))
    float TerrainRadius = 5000.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 3))
    int32 NumPaths = 3;

    // How many straight segments each path is subdivided into before noise is applied
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 4, UIMin = 4))
    int32 PathSegments = 24;

    // Max perpendicular offset (world units) the path can wander from the straight line to the tower
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 0.0, UIMin = 0.0))
    float PathWanderAmount = 1100.0f;

    // How rapidly the wander offset changes along the path. Lower = broader, slower sweeping curves;
    // higher = the path reverses direction between control points more often (wigglier)
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 0.01, UIMin = 0.01))
    float PathWanderFrequency = 0.2f;

    // How many smooth points to generate between each pair of control points. 1 = no smoothing, higher = smoother curve
    // but more distance/mesh checks per path
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (ClampMin = 1, UIMin = 1))
    int32 SplineSubdivisions = 8;

    // Half-width of the flat walkable strip each path carves through the terrain
    UPROPERTY(EditAnywhere, Category = "Terrain Generation",
              meta = (ClampMin = 50.0, UIMin = 150.0, Units = "Centimeters"))
    float PathWidth = 150.0f;

    // Minimum angular gap (degrees) enforced between neighbouring path entry points
    UPROPERTY(EditAnywhere, Category = "Terrain Generation",
              meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 180.0, UIMax = 180.0, Units = "Degrees"))
    float MinEntryAngleSeparation = 25.0f;

    // Minimum gap enforced between two different paths' corridor edges. Paths built after the first steer away from
    // every already-built path so they generally stop crossing/overlapping each other in the middle of the map
    UPROPERTY(EditAnywhere, Category = "Terrain Generation",
              meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float MinPathSeparation = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation")
    bool bRandomSeedEachGame = true;

    // Generate using a specific seed
    UPROPERTY(EditAnywhere, Category = "Terrain Generation", meta = (EditCondition = "!bRandomSeedEachGame"))
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug")
    bool bDrawDebugPaths = false;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug", meta = (EditCondition = "bDrawDebugPaths"))
    float DebugDrawDuration = 30.0f;

    // World units per grid cell (smaller = denser mesh/more triangles)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 50.0, UIMin = 50.0, Units = "Centimeters"))
    float CellSize = 100.0f;

    // World units of vertical relief added by noise, before path flattening
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0, UIMin = 0.0))
    float HeightAmplitude = 250.0f;

    // Larger = more gradual hills (lower frequency noise)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 100.0, UIMin = 100.0))
    float NoiseWavelength = 2500.0f;

    // Number of fractal noise layers combined together (higher = rougher/more detailed terrain)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 1, UIMin = 1, ClampMax = 6, UIMax = 6))
    int32 NoiseOctaves = 2;

    // Extra distance beyond each path's Width that stays completely flat (same PathHeight as the
    // corridor itself) before the height blend below even starts
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float PathFlatZoneWidth = 500.0f;

    // Distance beyond PathFlatZoneWidth over which terrain HEIGHT ramps from flat up to full noise
    // height, creating a "gradually bumpier farther out" transition
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float PathHeightBlendWidth = 700.0f;

    // Extra distance beyond each path's Width over which the vertex-colour mask (and so the material's
    // path/terrain blend) transitions from path look to terrain look. Independent of
    // PathHeightBlendWidth so the texture edge can be tighter (or wider) than the height ramp
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float PathTextureBlendWidth = 50.0f;

    // World Z of the flat ground along paths
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh")
    float PathHeight = 0.0f;

    // Used to create a purely visual path around the tower, doesn't actually affect the path
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float TowerRadius = 300.0f;

    // Should blend between a path look and a terrain look using the vertex-colour mask GenerateTerrain()
    // bakes in (VertexColor.R: 0 = path, 1 = terrain)
    UPROPERTY(EditAnywhere, Category = "Terrain Mesh")
    UMaterialInterface* TerrainMaterial;

    UPROPERTY(EditAnywhere, Category = "Defender Spots")
    TSubclassOf<ADefenderSpot> DefenderSpotClass;

    // Arc-length distance walked along a path between candidate spot positions
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotSpacing = 500.0f;

    // Extra clearance beyond the path's flattened corridor edge before placing a spot
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotOffset = 150.0f;

    // Minimum distance enforced between two spots so their colliders never overlap
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotMinSeparation = 400.0f;

    // If a candidate spot position gets rejected (too close to another spot or a path), how many times
    // to retry nearby before giving up on that stretch of path entirely. Without this, a path that
    // curves sharply can end up with long stretches with no defender spots at all
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0, UIMin = 0))
    int32 DefenderSpotPlacementRetries = 4;

    // How far along the path to nudge a rejected candidate before retrying
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotRetryStep = 100.0f;

    // Footprint radius of a defender spot. Used to keep spots clear of the path (with DefenderSpotOffset)
    // and to place them at a consistent distance a defender can actually reach the path from
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotRadius = 200.0f;

    // Extra distance beyond DefenderSpotRadius that stays completely flat before the height blend even starts
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotFlatZoneWidth = 150.0f;

    // Distance beyond DefenderSpotRadius + DefenderSpotFlatZoneWidth over which terrain height ramps
    // back up to full noise height
    UPROPERTY(EditAnywhere, Category = "Defender Spots", meta = (ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float DefenderSpotHeightBlendWidth = 400.0f;

    UPROPERTY(EditAnywhere, Category = "Terrain Generation|Debug")
    bool bDrawDebugDefenderSpots = false;

    UPROPERTY(EditAnywhere, Category = "Nature")
    bool bGenerateNature = true;

    // Grass is the same mesh everywhere, so it only needs the one entry
    UPROPERTY(EditAnywhere, Category = "Nature|Grass")
    FProceduralGrass Grass;

    UPROPERTY(EditAnywhere, Category = "Nature|Grass")
    FNatureLayer GrassLayer;

    // Mushrooms, flowers, ferns... each entry's SpawnChance makes it rarer
    UPROPERTY(EditAnywhere, Category = "Nature|Foliage")
    TArray<FProceduralFoliage> Foliage;

    UPROPERTY(EditAnywhere, Category = "Nature|Foliage")
    FNatureLayer FoliageLayer;

    UPROPERTY(EditAnywhere, Category = "Nature|Trees")
    TArray<FProceduralTree> Trees;

    UPROPERTY(EditAnywhere, Category = "Nature|Trees")
    FNatureLayer TreeLayer;

    // Size of the areas that favour one tree species, so similar trees clump together
    UPROPERTY(EditAnywhere, Category = "Nature|Trees", meta = (ClampMin = 100.0, UIMin = 100.0, Units = "Centimeters"))
    float TreeSpeciesWavelength = 1200.0f;

    // Chance a tree ignores the species favoured at its spot and picks any species, so a patch has some variety
    UPROPERTY(EditAnywhere, Category = "Nature|Trees",
              meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
    float TreeVarietyChance = 0.25f;

    // These block the navmesh, so keep RockLayer's Clearance generous
    UPROPERTY(EditAnywhere, Category = "Nature|Rocks")
    TArray<FProceduralRock> Rocks;

    UPROPERTY(EditAnywhere, Category = "Nature|Rocks")
    FNatureLayer RockLayer;

private:
    // Noise (0-1) at a world X/Y. Offset moves where on the noise field we sample, so each layer uses a different
    // part of it and the layers don't line up with each other or with the terrain height noise
    static float SampleNatureNoise(const FVector2D& WorldXY, float Wavelength, const FVector2D& Offset);

    // Steepness of the terrain at a world X/Y in degrees, from the same heightfield the mesh is built from
    float GetSlopeDegrees(const FVector2D& WorldXY) const;

    // 0 = too close to a path/tower plaza/defender spot to spawn at all, 1 = far enough for full density
    float GetNatureClearanceMultiplier(const FVector2D& WorldXY, const FNatureClearance& Clearance) const;

    // Walks a shuffled, jittered grid over the whole terrain. Each candidate that survives the layer's patch, density,
    // clearance and slope checks is handed to TryPlace, which decides what actually goes there (false = rejected)
    void ScatterLayer(const FNatureLayer& Layer, const FVector2D& NoiseOffset, FRandomStream& Stream,
                      TFunctionRef<bool(const FVector& Location, FRandomStream& Stream)> TryPlace) const;

    // One instanced component per mesh. bSolid = collides and affects the navmesh (trees, rocks)
    void CommitNatureInstances(const TMap<UStaticMesh*, TArray<FTransform>>& Instances, const FNatureLayer& Layer,
                               bool bSolid);

    void ClearNature();

    UPROPERTY()
    TArray<UHierarchicalInstancedStaticMeshComponent*> NatureComponents;

    // Fractal (multi-octave) Perlin height sample at a world X/Y, before any path flattening
    float SampleNoiseHeight(const FVector2D& WorldXY) const;

    // Signed distance from Point to the nearest path corridor's edge
    float DistanceToNearestPathEdge(const FVector2D& Point) const;

    // Signed distance from Point to the edge of the purely-visual circular tower plaza
    float DistanceToTowerEdge(const FVector2D& Point) const;

    // Signed distance from Point to the nearest defender spot's footprint edge
    float DistanceToNearestDefenderSpotEdge(const FVector2D& Point) const;

    // Height and vertex-colour/texture blend alpha at a world X/Y. Computed together since GenerateTerrain()
    // needs both per vertex and shouldn't pay for the distance checks twice
    void SampleTerrainPoint(const FVector2D& WorldXY, float& OutHeight, float& OutTextureBlendAlpha) const;

    // Picks NumPaths angles around the circle, evenly spaced then jittered, respecting MinEntryAngleSeparation
    TArray<float> GenerateEntryAngles(const FRandomStream& Stream) const;

    FTerrainPath BuildPath(float EntryAngleDegrees, const FRandomStream& Stream) const;

    // The coarse, noisy, straight-segment-connected points BuildPath used to return directly
    TArray<FVector> BuildPathControlPoints(float EntryAngleDegrees, const FRandomStream& Stream) const;

    // How far, and in which direction, a candidate point on the path currently being built should
    // be nudged to keep MinPathSeparation from every already-built path in Paths. Zero vector if
    // it's already far enough from all of them
    FVector ComputePathSeparationOffset(const FVector& Point) const;

    // Replace the sharp corners where the path would visibly bounce off its wander target with smooth curves
    TArray<FVector> SmoothPathControlPoints(const TArray<FVector>& ControlPoints) const;

    void DrawDebugForPaths() const;

    // Walks every path and works out valid defender spot locations along it, populating DefenderSpotLocations
    void ComputeDefenderSpotLocations();

    // Tries BasePoint + Offset, and if that's rejected, nudges BasePoint along SegDir (alternating
    // forward/back, growing each pair of attempts) and retries up to DefenderSpotPlacementRetries times
    void TryAddDefenderSpotNearby(const FVector& BasePoint, const FVector& SegDir, const FVector& Offset);

    // Adds Candidate to DefenderSpotLocations if it clears every path corridor (by DefenderSpotRadius +
    // DefenderSpotOffset) and every location already in DefenderSpotLocations (by DefenderSpotMinSeparation).
    // Returns whether it was actually added
    bool TryAddDefenderSpotLocation(const FVector2D& Candidate);

    // Destroys any previously spawned defender spots and spawns fresh ones at DefenderSpotLocations,
    // sampling the (by then hopefully already-flattened) terrain height at each one
    void SpawnDefenderSpots();

    void DrawDebugForDefenderSpots() const;

    UPROPERTY(BlueprintReadOnly, Category = "Terrain Generation", meta = (AllowPrivateAccess = true))
    TArray<FTerrainPath> Paths;

    UPROPERTY(BlueprintReadOnly, Category = "Defender Spots", meta = (AllowPrivateAccess = true))
    TArray<FVector2D> DefenderSpotLocations;

    UPROPERTY(BlueprintReadOnly, Category = "Defender Spots", meta = (AllowPrivateAccess = true))
    TArray<ADefenderSpot*> DefenderSpots;

    UPROPERTY(EditDefaultsOnly, Category = "Defender Spots",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float ExampleDefenderPerceptionRadius = 750.0f;
};
