// ReSharper disable CppParameterMayBeConst
#include "ProceduralGen/ProceduralTerrainGen.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/BodySetup.h"
#include "ProceduralMeshConversion.h"
#include "StaticMeshDescription.h"
#endif

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "CustomLog.h"
#include "KismetProceduralMeshLibrary.h"
#include "NavigationSystem.h"
#include "ProceduralMeshComponent.h"
#include "TDCollisionChannels.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"

namespace
{
    // Spatial hash of circles on the XY plane, so an overlap check only looks at nearby placements rather than all of
    // them
    class FNatureOccupancy
    {
    public:
        bool IsFree(const FVector2D& Point, float Radius) const
        {
            const int32 Reach = FMath::CeilToInt((Radius + LargestRadius) / CellSize);
            const FIntPoint Centre = ToCell(Point);
            for (int32 Y = -Reach; Y <= Reach; ++Y)
            {
                for (int32 X = -Reach; X <= Reach; ++X)
                {
                    const TArray<FCircle>* Circles = Cells.Find(FIntPoint(Centre.X + X, Centre.Y + Y));
                    if (!Circles) continue;
                    for (const FCircle& Circle : *Circles)
                        if (FVector2D::DistSquared(Point, Circle.Centre) < FMath::Square(Radius + Circle.Radius))
                            return false;
                }
            }
            return true;
        }

        void Add(const FVector2D& Point, float Radius)
        {
            Cells.FindOrAdd(ToCell(Point)).Add(FCircle {Point, Radius});
            LargestRadius = FMath::Max(LargestRadius, Radius);
        }

    private:
        struct FCircle
        {
            FVector2D Centre;
            float Radius;
        };

        static constexpr float CellSize = 300.0f;

        static FIntPoint ToCell(const FVector2D& Point)
        {
            return FIntPoint(FMath::FloorToInt(Point.X / CellSize), FMath::FloorToInt(Point.Y / CellSize));
        }

        float LargestRadius = 0.0f;
        TMap<FIntPoint, TArray<FCircle>> Cells;
    };

    // A random scale in [Min, Max], tolerating them being entered the wrong way round
    float RandomScale(FRandomStream& Stream, float MinScale, float MaxScale)
    {
        return Stream.FRandRange(FMath::Min(MinScale, MaxScale), FMath::Max(MinScale, MaxScale));
    }
} // namespace

AProceduralTerrainGen::AProceduralTerrainGen()
{
    // Starting points for each kind of nature, tune per level
    GrassLayer.Spacing = 110.0f;
    GrassLayer.Density = 0.9f;
    GrassLayer.Clearance = {40.0f, 500.0f, 0.5f}; // Slightly thinner near paths, but never on them
    GrassLayer.CullDistance = 3500.0f;
    GrassLayer.MaxInstances = 30000;

    FoliageLayer.Spacing = 160.0f;
    FoliageLayer.Density = 0.7f;
    FoliageLayer.PatchWavelength = 900.0f;
    FoliageLayer.PatchThreshold = 0.58f;
    FoliageLayer.Clearance = {100.0f, 700.0f, 0.25f}; // Noticeably thinner near paths
    FoliageLayer.CullDistance = 3500.0f;
    FoliageLayer.MaxInstances = 3000;

    TreeLayer.Spacing = 300.0f;
    TreeLayer.Density = 0.55f;
    TreeLayer.PatchWavelength = 2200.0f;
    TreeLayer.PatchThreshold = 0.58f;
    TreeLayer.Clearance = {800.0f, 700.0f, 0.4f}; // Well away from paths and spots
    TreeLayer.MaxSlope = 20.0f;
    TreeLayer.VerticalOffset = -10.0f;
    TreeLayer.CullDistance = 6000.0f;
    TreeLayer.MaxInstances = 500;

    RockLayer.Spacing = 400.0f;
    RockLayer.Density = 0.4f;
    RockLayer.PatchWavelength = 1600.0f;
    RockLayer.PatchThreshold = 0.5f;
    RockLayer.Clearance = {700.0f, 500.0f, 0.5f}; // They block the navmesh, so keep them away from the path
    RockLayer.MaxSlope = 25.0f;
    RockLayer.VerticalOffset = -5.0f;
    RockLayer.CullDistance = 5000.0f;
    RockLayer.MaxInstances = 300;

    PrimaryActorTick.bCanEverTick = false;

    TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
    SetRootComponent(TerrainMesh);
    TerrainMesh->SetMobility(EComponentMobility::Static); // Never moves after BeginPlay, lets the engine optimise it
    TerrainMesh->bUseAsyncCooking = false;
    TerrainMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TerrainMesh->SetCollisionObjectType(ECC_WorldStatic);
    TerrainMesh->SetCollisionResponseToAllChannels(ECR_Block);

    // Starts hidden/non-colliding with no mesh assigned, BakeMesh() fills it in and swaps it for TerrainMesh
    BakedTerrainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BakedTerrainMesh"));
    BakedTerrainMesh->SetupAttachment(TerrainMesh);
    BakedTerrainMesh->SetMobility(EComponentMobility::Static);
    BakedTerrainMesh->SetVisibility(false);
    BakedTerrainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProceduralTerrainGen::BeginPlay()
{
    Super::BeginPlay();
    GeneratePaths();
    ComputeDefenderSpotLocations(); // Before GenerateTerrain() so it can flatten around them too
    GenerateTerrain();
    SpawnDefenderSpots();
    GenerateNature(); // After the spots exist so it can keep clear of them, and before the navmesh is rebuilt
#if WITH_EDITOR
    BakeMesh();
    // ^ Uses editor only stuff to bake the mesh into a static mesh. Keep doing in editor for better performance in
    // editor, but leave out of build (because impossible in build) which has better performance already
#else
    if (IsValid(TerrainMesh)) UNavigationSystemV1::UpdateComponentInNavOctree(*TerrainMesh);
    // ^ Need to call this separate in build because it was originally done in BakeMesh()
#endif
    RebuildNavMesh();
    TD_LOG_INFO(TEXT("Level is finished generating :)"));
}

void AProceduralTerrainGen::GeneratePaths()
{
    if (bRandomSeedEachGame) Seed = FMath::Rand();

    TD_LOG_INFO(TEXT("Current Seed: %d"), Seed);

    TD_LOG_INFO_NDSP(TEXT("Generating Paths..."));

    const FRandomStream Stream(Seed);

    Paths.Empty(NumPaths); // Clears any previous roll and pre-allocates for the new one

    // ReSharper disable once CppTooWideScopeInitStatement
    const TArray<float> EntryAngles = GenerateEntryAngles(Stream);
    for (const float Angle : EntryAngles) Paths.Add(BuildPath(Angle, Stream));

    TD_LOG_INFO_NDSP(TEXT("Generated Paths"));

    if (bDrawDebugPaths) DrawDebugForPaths();
}

TArray<float> AProceduralTerrainGen::GenerateEntryAngles(const FRandomStream& Stream) const
{
    TArray<float> Angles;
    Angles.Reserve(NumPaths);

    const float Spacing = 360.0f / NumPaths; // Even split around the circle as a starting point

    // Half the leftover room either side of Spacing, clamped so paths can never jitter closer than
    // MinEntryAngleSeparation
    const float MaxJitter = FMath::Max(0.0f, (Spacing - MinEntryAngleSeparation) * 0.5f);

    for (int32 i = 0; i < NumPaths; ++i)
    {
        const float BaseAngle = i * Spacing;
        const float Jitter = Stream.FRandRange(-MaxJitter, MaxJitter);
        Angles.Add(FMath::Fmod(BaseAngle + Jitter + 360.0f, 360.0f)); // +360 because Jitter can go negative
    }

    return Angles;
}

FTerrainPath AProceduralTerrainGen::BuildPath(float EntryAngleDegrees, const FRandomStream& Stream) const
{
    FTerrainPath Path;
    Path.Width = PathWidth;
    Path.Points = SmoothPathControlPoints(BuildPathControlPoints(EntryAngleDegrees, Stream));
    for (FVector& Point : Path.Points) Point.Z = GetTerrainHeight(FVector2D(Point));
    return Path;
}

TArray<FVector> AProceduralTerrainGen::BuildPathControlPoints(float EntryAngleDegrees,
                                                              const FRandomStream& Stream) const
{
    TArray<FVector> ControlPoints;
    ControlPoints.Reserve(PathSegments + 1);

    const float AngleRadians = FMath::DegreesToRadians(EntryAngleDegrees);
    const FVector EnemySpawn(TerrainRadius * FMath::Cos(AngleRadians), TerrainRadius * FMath::Sin(AngleRadians), 0.0f);
    const FVector TowerPos = FVector::ZeroVector;

    const FVector Direction = (TowerPos - EnemySpawn).GetSafeNormal();
    const FVector Perpendicular(-Direction.Y, Direction.X, 0.0f); // Used to bow the path sideways

    // Offsets this path's noise sample range so different paths don't wander in similar patterns
    const float NoiseSeedOffset = Stream.FRandRange(0.0f, 1000.0f);

    for (int32 i = 0; i <= PathSegments; ++i)
    {
        const float Alpha = static_cast<float>(i) / PathSegments;
        const FVector BasePoint = FMath::Lerp(EnemySpawn, TowerPos, Alpha);

        // Tapers to zero at both ends so the path still starts on the edge and ends exactly at the tower
        const float Taper = FMath::Sin(PI * Alpha);
        const float NoiseSample = FMath::PerlinNoise1D(Alpha * PathSegments * PathWanderFrequency + NoiseSeedOffset);

        const FVector Offset =
            Perpendicular * NoiseSample * PathWanderAmount * Taper; // Sideways only, never changes path length/Z
        FVector Point = BasePoint + Offset;

        // Tapered the same as the wander offset above
        // Every path still has to meet exactly at the tower and start exactly at its own edge spawn
        // So this can't be allowed to push those two ends around
        Point += ComputePathSeparationOffset(Point) * Taper;

        ControlPoints.Add(Point);
    }

    return ControlPoints;
}

// Checks Point against every already-built path in Paths and returns how far it should be moved away to keep at least
// MinPathSeparation clear of each one's corridor edge
FVector AProceduralTerrainGen::ComputePathSeparationOffset(const FVector& Point) const
{
    FVector TotalPush = FVector::ZeroVector;

    for (const auto& [Points, Width] : Paths)
    {
        float ClosestEdgeDistance = TNumericLimits<float>::Max();
        FVector ClosestPoint = FVector::ZeroVector;

        for (const FVector& OtherPoint : Points)
        {
            const float EdgeDistance = FVector::Dist2D(Point, OtherPoint) - PathWidth - Width;
            if (EdgeDistance < ClosestEdgeDistance)
            {
                ClosestEdgeDistance = EdgeDistance;
                ClosestPoint = OtherPoint;
            }
        }

        if (ClosestEdgeDistance >= MinPathSeparation) continue; // Already far enough from this path

        FVector Away = Point - ClosestPoint;
        Away.Z = 0.0f;
        if (!Away.Normalize()) continue; // Sitting exactly on the other path, no defined direction to push

        TotalPush += Away * (MinPathSeparation - ClosestEdgeDistance);
    }

    return TotalPush;
}

// Catmull-Rom smoothing
TArray<FVector> AProceduralTerrainGen::SmoothPathControlPoints(const TArray<FVector>& ControlPoints) const
{
    if (ControlPoints.Num() < 2) return ControlPoints;

    TArray<FVector> Smoothed;
    const int32 LastIndex = ControlPoints.Num() - 1;
    Smoothed.Reserve(LastIndex * SplineSubdivisions + 1);

    for (int32 i = 0; i < LastIndex; ++i)
    {
        const FVector& P1 = ControlPoints[i];
        const FVector& P2 = ControlPoints[i + 1];

        const FVector P0 = i > 0 ? ControlPoints[i - 1] : P1 * 2.0f - P2;
        const FVector P3 = i < LastIndex - 1 ? ControlPoints[i + 2] : P2 * 2.0f - P1;

        const float T0 = 0.0f;
        const float T1 = T0 + FMath::Sqrt(FVector::Dist(P0, P1));
        const float T2 = T1 + FMath::Sqrt(FVector::Dist(P1, P2));
        const float T3 = T2 + FMath::Sqrt(FVector::Dist(P2, P3));

        const int32 NumSteps = i == LastIndex - 1 ? SplineSubdivisions + 1 : SplineSubdivisions;
        for (int32 Step = 0; Step < NumSteps; ++Step)
        {
            const float T = T1 + (T2 - T1) * (static_cast<float>(Step) / SplineSubdivisions);
            Smoothed.Add(FMath::CubicCRSplineInterp(P0, P1, P2, P3, T0, T1, T2, T3, T));
        }
    }

    return Smoothed;
}

void AProceduralTerrainGen::GenerateTerrain() const
{
    if (!IsValid(TerrainMesh)) return;

    TD_LOG_INFO_NDSP(TEXT("Generating Terrain..."));

    TerrainMesh->ClearAllMeshSections();

    // Extends past TerrainRadius so path wander/width/blend never runs off the edge of the grid
    const float HalfExtent = GetTerrainHalfExtent();
    const int32 NumCells = FMath::Max(1, FMath::CeilToInt(HalfExtent * 2.0f / CellSize)); // At least 1 cell
    const int32 VertsPerSide = NumCells + 1; // N cells needs N+1 verts per row/column

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    // R channel is the path/terrain texture blend mask: 0 inside a path corridor, ramping up to 1 by
    // PathTextureBlendWidth outside
    TArray<FLinearColor> VertexColors;

    Vertices.Reserve(VertsPerSide * VertsPerSide);
    UVs.Reserve(VertsPerSide * VertsPerSide);
    VertexColors.Reserve(VertsPerSide * VertsPerSide);

    // Build the vertex grid, one vertex per (X, Y) cell corner, height sampled from the same
    // heightfield GetTerrainHeight() exposes, so a spot placed later at some WorldXY will always
    // read back the exact Z this mesh actually has there
    for (int32 Y = 0; Y < VertsPerSide; ++Y)
    {
        for (int32 X = 0; X < VertsPerSide; ++X)
        {
            const float WorldX = -HalfExtent + X * CellSize; // Grid is centred on the tower
            const float WorldY = -HalfExtent + Y * CellSize;
            const FVector2D WorldXY(WorldX, WorldY);

            float Height, TextureBlendAlpha;
            SampleTerrainPoint(WorldXY, Height, TextureBlendAlpha);

            Vertices.Add(FVector(WorldX, WorldY, Height));
            UVs.Add(WorldXY / 1000.0f);
            VertexColors.Add(FLinearColor(TextureBlendAlpha, TextureBlendAlpha, TextureBlendAlpha, 1.0f));
        }
    }

    // Two triangles per grid cell, wound for upward-facing normals
    Triangles.Reserve(NumCells * NumCells * 6);
    for (int32 Y = 0; Y < NumCells; ++Y)
    {
        for (int32 X = 0; X < NumCells; ++X)
        {
            const int32 Current = X + Y * VertsPerSide;
            const int32 Below = X + (Y + 1) * VertsPerSide;
            const int32 Right = X + 1 + Y * VertsPerSide;
            const int32 BelowRight = X + 1 + (Y + 1) * VertsPerSide;

            Triangles.Add(Current);
            Triangles.Add(Below);
            Triangles.Add(Right);

            Triangles.Add(Right);
            Triangles.Add(Below);
            Triangles.Add(BelowRight);
        }
    }

    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

    TerrainMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

#if !WITH_EDITOR
    if (TerrainMaterial) TerrainMesh->SetMaterial(0, TerrainMaterial);
    // ^ Need to apply material to procedural mesh as opposed to static mesh when in build
#endif

    TD_LOG_INFO_NDSP(TEXT("Generated Terrain"));
}

float AProceduralTerrainGen::GetTerrainHeight(const FVector2D& WorldXY) const
{
    float Height, TextureBlendAlpha;
    SampleTerrainPoint(WorldXY, Height, TextureBlendAlpha);
    return Height;
}

void AProceduralTerrainGen::SampleTerrainPoint(const FVector2D& WorldXY, float& OutHeight,
                                               float& OutTextureBlendAlpha) const
{
    const float NoiseHeight = SampleNoiseHeight(WorldXY); // What the height would be with no paths at all
    // Tower plaza folded in here so it blends exactly like an extra path, without actually being one
    const float PathEdgeDistance = FMath::Min(DistanceToNearestPathEdge(WorldXY), DistanceToTowerEdge(WorldXY));
    const float SpotEdgeDistance = DistanceToNearestDefenderSpotEdge(WorldXY); // Negative = inside a spot

    const float PathHeightAlpha = FMath::SmoothStep(0.0f, PathHeightBlendWidth, PathEdgeDistance - PathFlatZoneWidth);
    const float SpotHeightAlpha =
        FMath::SmoothStep(0.0f, DefenderSpotHeightBlendWidth, SpotEdgeDistance - DefenderSpotFlatZoneWidth);
    OutHeight = FMath::Lerp(PathHeight, NoiseHeight, FMath::Min(PathHeightAlpha, SpotHeightAlpha));

    OutTextureBlendAlpha = FMath::SmoothStep(0.0f, PathTextureBlendWidth, PathEdgeDistance);
}

// Stack several Perlin samples on top of each other, each one higher frequency and lower amplitude than the last, so
// the terrain gets both big rolling hills (low octaves) and small bumps (high octaves) instead of looking like one
// uniform wave
float AProceduralTerrainGen::SampleNoiseHeight(const FVector2D& WorldXY) const
{
    float Total = 0.0f;
    float AmplitudeSum = 0.0f; // Tracks max possible Total so can normalise back to -1..1 regardless of NoiseOctaves
    float Amplitude = 1.0f;
    float Frequency = 1.0f / NoiseWavelength;

    for (int32 Octave = 0; Octave < NoiseOctaves; ++Octave)
    {
        Total += FMath::PerlinNoise2D(WorldXY * Frequency) * Amplitude;
        AmplitudeSum += Amplitude;

        Amplitude *= 0.5f; // Each octave contributes half as much height as the last
        Frequency *= 2.0f; // But samples the noise twice as fast, i.e. finer detail
    }

    return AmplitudeSum > 0.0f ? Total / AmplitudeSum * HeightAmplitude : 0.0f;
}

// Checked against every path, not just the nearest one, because two paths can run
// close together and a point can be outside path A's corridor but still inside path B's
float AProceduralTerrainGen::DistanceToNearestPathEdge(const FVector2D& Point) const
{
    float MinEdgeDistance = TNumericLimits<float>::Max(); // No paths yet = treat as infinitely far from any corridor

    for (const auto& [Points, Width] : Paths)
    {
        // A path is a polyline, not one straight segment, so distance-to-path means distance to
        // whichever segment of it is closest
        for (int32 i = 0; i < Points.Num() - 1; ++i)
        {
            const FVector2D SegStart(Points[i]);
            const FVector2D SegEnd(Points[i + 1]);
            const FVector2D Closest = FMath::ClosestPointOnSegment2D(Point, SegStart, SegEnd);

            const float EdgeDistance = FVector2D::Distance(Point, Closest) - Width;
            MinEdgeDistance = FMath::Min(MinEdgeDistance, EdgeDistance);
        }
    }

    return MinEdgeDistance;
}

float AProceduralTerrainGen::DistanceToTowerEdge(const FVector2D& Point) const { return Point.Size() - TowerRadius; }

// Same reasoning as DistanceToNearestPathEdge, a point could be near more than one spot
float AProceduralTerrainGen::DistanceToNearestDefenderSpotEdge(const FVector2D& Point) const
{
    float MinEdgeDistance = TNumericLimits<float>::Max();

    for (const FVector2D& SpotLocation : DefenderSpotLocations)
        MinEdgeDistance = FMath::Min(MinEdgeDistance, FVector2D::Distance(Point, SpotLocation) - DefenderSpotRadius);

    return MinEdgeDistance;
}

void AProceduralTerrainGen::DrawDebugForPaths() const
{
#if WITH_EDITOR
    const UWorld* const World = GetWorld();
    if (!World) return;

    DrawDebugSphere(World, GetActorLocation(), PathWidth, 16, FColor::Red, false, DebugDrawDuration, 0, 4.0f);

    for (const auto& [Points, Width] : Paths)
    {
        for (int32 i = 0; i < Points.Num() - 1; ++i)
            DrawDebugLine(World, Points[i], Points[i + 1], FColor::Yellow, false, DebugDrawDuration, 0, 6.0f);

        if (Points.Num() > 0)
            DrawDebugSphere(World, Points[0], 80.0f, 12, FColor::Green, false, DebugDrawDuration, 0, 3.0f);
    }
#endif
}

void AProceduralTerrainGen::GenerateDefenderSpots()
{
    TD_LOG_INFO_NDSP(TEXT("Generating Defender Spots..."));
    ComputeDefenderSpotLocations();
    SpawnDefenderSpots();
    TD_LOG_INFO_NDSP(TEXT("Generated Defender Spots"));
}

void AProceduralTerrainGen::ComputeDefenderSpotLocations()
{
    DefenderSpotLocations.Empty();

    for (const auto& [Points, Width] : Paths)
    {
        // Stagger the first spot half a spacing in so it doesn't sit right on the edge spawn point
        float DistanceUntilNextSpot = DefenderSpotSpacing * 0.5f;

        // Walk the path's polyline segment by segment, treating it as one continuous line so spots end up evenly spaced
        // along the whole path rather than reset to 0 at every bend
        for (int32 i = 0; i < Points.Num() - 1; ++i)
        {
            FVector SegPoint = Points[i]; // Current walk position, advances along the segment below
            const FVector SegEnd = Points[i + 1];
            float SegRemaining = FVector::Dist(SegPoint, SegEnd);
            // Yes "KINDA_SMALL_NUMBER" is an actual UE macro lol, who needs epsilon?
            if (SegRemaining <= KINDA_SMALL_NUMBER) continue; // Don't divide by 0

            const FVector SegDir = (SegEnd - SegPoint) / SegRemaining;
            const FVector Perpendicular(-SegDir.Y, SegDir.X, 0.0f);

            // Fits as many spot positions as this segment has room for before falling through to the next segment
            while (DistanceUntilNextSpot <= SegRemaining)
            {
                SegPoint += SegDir * DistanceUntilNextSpot;
                SegRemaining -= DistanceUntilNextSpot;

                // Has to clear the path by the spot's own radius, not just its centre, otherwise the
                // spot's footprint can hang out over the walkable corridor even though its centre doesn't
                const float SpotDistance = Width + DefenderSpotRadius + DefenderSpotOffset;
                TryAddDefenderSpotNearby(SegPoint, SegDir, Perpendicular * SpotDistance); // Left
                TryAddDefenderSpotNearby(SegPoint, SegDir, Perpendicular * -SpotDistance); // Right

                DistanceUntilNextSpot = DefenderSpotSpacing; // Back to full spacing for the next spot
            }

            DistanceUntilNextSpot -= SegRemaining; // Carry the unused leftover distance into the next segment
        }
    }
}

void AProceduralTerrainGen::TryAddDefenderSpotNearby(const FVector& BasePoint, const FVector& SegDir,
                                                     const FVector& Offset)
{
    if (TryAddDefenderSpotLocation(FVector2D(BasePoint + Offset))) return;

    // First attempt got rejected find a nearby gap instead of just leaving this stretch of path with no spot at all
    for (int32 Attempt = 1; Attempt <= DefenderSpotPlacementRetries; ++Attempt)
    {
        const float NudgeDistance = FMath::CeilToFloat(Attempt / 2.0f) * DefenderSpotRetryStep;
        const float Direction = (Attempt % 2 == 1) ? 1.0f : -1.0f; // Alternates +Step, -Step, +2*Step, -2*Step...
        const FVector Candidate = BasePoint + SegDir * (NudgeDistance * Direction) + Offset;
        if (TryAddDefenderSpotLocation(FVector2D(Candidate))) return;
    }
}

bool AProceduralTerrainGen::TryAddDefenderSpotLocation(const FVector2D& Candidate)
{
    // Reject if the spot's own footprint would overlap any path's walkable corridor
    const float RequiredPathClearance = DefenderSpotRadius + DefenderSpotOffset;
    if (DistanceToNearestPathEdge(Candidate) < RequiredPathClearance) return false;

    // Reject if it's too close to a spot already found this call
    for (const FVector2D& Existing : DefenderSpotLocations)
    {
        const float DeltaX = FMath::Abs(Existing.X - Candidate.X);
        const float DeltaY = FMath::Abs(Existing.Y - Candidate.Y);
        if (FMath::Max(DeltaX, DeltaY) < DefenderSpotMinSeparation) return false;
    }

    DefenderSpotLocations.Add(Candidate);
    return true;
}

void AProceduralTerrainGen::SpawnDefenderSpots()
{
    TD_LOG_INFO_NDSP(TEXT("Spawning Defender Spots..."));
    for (ADefenderSpot* Spot : DefenderSpots)
    {
        if (IsValid(Spot)) Spot->Destroy();
    }
    DefenderSpots.Empty();

    if (!DefenderSpotClass) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (const FVector2D& Location : DefenderSpotLocations)
    {
        const FVector SpawnLocation(Location.X, Location.Y, GetTerrainHeight(Location));
        if (ADefenderSpot* NewSpot = GetWorld()->SpawnActor<ADefenderSpot>(DefenderSpotClass, SpawnLocation,
                                                                           FRotator::ZeroRotator, SpawnParams))
            DefenderSpots.Add(NewSpot);
    }

    if (bDrawDebugDefenderSpots) DrawDebugForDefenderSpots();
    TD_LOG_INFO_NDSP(TEXT("Spawned Defender Spots"));
}

// This was very hard to figure out, because not much information about how to do this, but I managed to get
// it working :)
#if WITH_EDITOR
void AProceduralTerrainGen::BakeMesh()
{
    if (!IsValid(TerrainMesh) || !IsValid(BakedTerrainMesh)) return;

    TD_LOG_INFO_NDSP(TEXT("Baking Mesh..."));

    UStaticMesh* StaticMesh =
        NewObject<UStaticMesh>(this /* = Owner/Holder */, NAME_None, RF_Transient /* = Don't save mesh */);
    StaticMesh->bAllowCPUAccess = true; // Keep mesh data accessible to RAM and not only VRAM
    StaticMesh->NeverStream = true; // Idk but it does something
    StaticMesh->SetLightingGuid(); // Needed to work with lighting I think
    StaticMesh->AddSourceModel(); // Needed to be able to have a model I think

    // Create mesh description based on procedural mesh, this is passed to static mesh to build it
    const FMeshDescription MeshDescription = BuildMeshDescription(TerrainMesh);
    UStaticMeshDescription* SMDesc = StaticMesh->CreateStaticMeshDescription();
    SMDesc->SetMeshDescription(MeshDescription);

    // Add the material as an option for the mesh
    if (TerrainMaterial) StaticMesh->GetStaticMaterials().Add(FStaticMaterial(TerrainMaterial));

    // Build mesh                               vvv TArray, but only need one description
    StaticMesh->BuildFromStaticMeshDescriptions({SMDesc}, false);
    // .                                                  ^^^ don't build simple flat collision
    // ^ the . is because clang-format doesn't let me have only spaces :(

    // Set the material
    if (TerrainMaterial) StaticMesh->SetMaterial(0, TerrainMaterial);

    // Build collision
    if (UBodySetup* BodySetup = StaticMesh->GetBodySetup())
    {
        BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
        BodySetup->InvalidatePhysicsData();
        BodySetup->CreatePhysicsMeshes();
    }

    // Set the stuff
    BakedTerrainMesh->SetStaticMesh(StaticMesh);
    BakedTerrainMesh->SetCollisionObjectType(TerrainMesh->GetCollisionObjectType());
    BakedTerrainMesh->SetCollisionEnabled(TerrainMesh->GetCollisionEnabled());
    BakedTerrainMesh->SetCollisionResponseToChannels(TerrainMesh->GetCollisionResponseToChannels());
    BakedTerrainMesh->SetVisibility(true);

    // Clear the stuff to save memory
    TerrainMesh->ClearAllMeshSections();
    TerrainMesh->SetVisibility(false);
    TerrainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UNavigationSystemV1::UpdateComponentInNavOctree(*TerrainMesh);
    UNavigationSystemV1::UpdateComponentInNavOctree(*BakedTerrainMesh);

    TD_LOG_INFO_NDSP(TEXT("Baked Mesh"));
}
#else
void AProceduralTerrainGen::BakeMesh()
{
    TD_LOG_WARN(TEXT(
        "AProceduralTerrainGen::BakeMesh -> disabled outside the editor (uses editor-only UStaticMesh APIs), terrain stays as the raw procedural mesh"));
}
#endif

void AProceduralTerrainGen::RebuildNavMesh() const
{
    if (UWorld* World = GetWorld())
    {
        TD_LOG_INFO_NDSP(TEXT("Rebuilding Nav Mesh..."));
        FNavigationSystem::Build(*World);
        TD_LOG_INFO_NDSP(TEXT("Rebuilt Nav Mesh"));
    }
}

void AProceduralTerrainGen::DrawDebugForDefenderSpots() const
{
#if WITH_EDITOR
    const UWorld* const World = GetWorld();
    if (!World) return;

    for (const ADefenderSpot* Spot : DefenderSpots)
    {
        if (IsValid(Spot))
            DrawDebugSphere(World, Spot->GetActorLocation(), ExampleDefenderPerceptionRadius, 12, FColor::Cyan, false,
                            DebugDrawDuration, 0, 3.0f);
    }
#endif
}

// Perlin noise only ever comes out as roughly -0.7..0.7, so stretch that to fill 0..1
float AProceduralTerrainGen::SampleNatureNoise(const FVector2D& WorldXY, float Wavelength, const FVector2D& Offset)
{
    const float Noise = FMath::PerlinNoise2D((WorldXY + Offset) / Wavelength);
    return FMath::Clamp((Noise + 0.7f) / 1.4f, 0.0f, 1.0f);
}

float AProceduralTerrainGen::GetSlopeDegrees(const FVector2D& WorldXY) const
{
    const float Step = FMath::Max(CellSize, 50.0f); // Same scale as the mesh's own triangles
    const float SlopeX =
        (GetTerrainHeight(WorldXY + FVector2D(Step, 0.0f)) - GetTerrainHeight(WorldXY - FVector2D(Step, 0.0f))) /
        (2.0f * Step);
    const float SlopeY =
        (GetTerrainHeight(WorldXY + FVector2D(0.0f, Step)) - GetTerrainHeight(WorldXY - FVector2D(0.0f, Step))) /
        (2.0f * Step);
    return FMath::RadiansToDegrees(FMath::Atan(FMath::Sqrt(SlopeX * SlopeX + SlopeY * SlopeY)));
}

float AProceduralTerrainGen::GetNatureClearanceMultiplier(const FVector2D& WorldXY,
                                                          const FNatureClearance& Clearance) const
{
    // Tower area counts as a path, the same way SampleTerrainPoint treats it
    const float PathDistance = FMath::Min(DistanceToNearestPathEdge(WorldXY), DistanceToTowerEdge(WorldXY));
    const float Distance = FMath::Min(PathDistance, DistanceToNearestDefenderSpotEdge(WorldXY));
    if (Distance < Clearance.MinDistance) return 0.0f;
    if (Clearance.FalloffDistance <= 0.0f) return 1.0f;
    const float Alpha =
        FMath::SmoothStep(Clearance.MinDistance, Clearance.MinDistance + Clearance.FalloffDistance, Distance);
    return FMath::Lerp(Clearance.NearDensity, 1.0f, Alpha);
}

void AProceduralTerrainGen::ScatterLayer(const FNatureLayer& Layer, const FVector2D& NoiseOffset, FRandomStream& Stream,
                                         TFunctionRef<bool(const FVector&, FRandomStream&)> TryPlace) const
{
    const float HalfExtent = GetTerrainHalfExtent();
    const float Spacing = FMath::Max(Layer.Spacing, 25.0f);
    const int32 CellsPerSide = FMath::Max(1, FMath::FloorToInt(HalfExtent * 2.0f / Spacing));

    // Shuffled so that if MaxInstances is hit, the instances that did spawn are spread over the whole map instead of
    // all being packed into whichever corner the loop started in
    TArray<int32> Order;
    Order.Reserve(CellsPerSide * CellsPerSide);
    for (int32 i = 0; i < CellsPerSide * CellsPerSide; ++i) Order.Add(i);
    for (int32 i = Order.Num() - 1; i > 0; --i) Order.Swap(i, Stream.RandRange(0, i));

    int32 Placed = 0;
    for (const int32 Index : Order)
    {
        if (Placed >= Layer.MaxInstances) break;

        const int32 CellX = Index % CellsPerSide;
        const int32 CellY = Index / CellsPerSide;
        const FVector2D Point(-HalfExtent + (CellX + Stream.FRand()) * Spacing,
                              -HalfExtent + (CellY + Stream.FRand()) * Spacing);

        float Chance = Layer.Density;
        if (Layer.PatchWavelength > 0.0f)
        {
            const float PatchNoise = SampleNatureNoise(Point, Layer.PatchWavelength, NoiseOffset);
            Chance *= FMath::SmoothStep(Layer.PatchThreshold, Layer.PatchThreshold + 0.1f, PatchNoise);
        }
        if (Stream.FRand() >= Chance) continue;

        if (Stream.FRand() >= GetNatureClearanceMultiplier(Point, Layer.Clearance)) continue;

        // Slope costs four extra height samples, so only worth it for layers that actually have a limit
        if (Layer.MaxSlope < 90.0f && GetSlopeDegrees(Point) > Layer.MaxSlope) continue;

        const FVector Location(Point.X, Point.Y, GetTerrainHeight(Point) + Layer.VerticalOffset);
        if (TryPlace(Location, Stream)) ++Placed;
    }
}

void AProceduralTerrainGen::CommitNatureInstances(const TMap<UStaticMesh*, TArray<FTransform>>& Instances,
                                                  const FNatureLayer& Layer, bool bSolid)
{
    for (const auto& [Mesh, Transforms] : Instances)
    {
        if (!Mesh || Transforms.IsEmpty()) continue;

        auto* Component = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
        Component->SetStaticMesh(Mesh);
        Component->SetupAttachment(GetRootComponent());
        Component->SetGenerateOverlapEvents(false);
        const int32 CullDistance = FMath::RoundToInt(Layer.CullDistance);
        Component->SetCullDistances(CullDistance, CullDistance);

        if (bSolid)
        {
            Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Component->SetCollisionObjectType(ECC_WorldStatic);
            Component->SetCollisionResponseToAllChannels(ECR_Block);
            Component->SetCollisionResponseToChannel(MouseClickTraceChannel, ECR_Ignore);
            Component->SetCanEverAffectNavigation(true);
        }
        else
        {
            Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Component->SetCanEverAffectNavigation(false);
            Component->SetCastShadow(false);
        }

        Component->RegisterComponent();
        Component->PreAllocateInstancesMemory(Transforms.Num());
        Component->AddInstances(Transforms, false, true, false);
        if (bSolid) UNavigationSystemV1::UpdateComponentInNavOctree(*Component);
        NatureComponents.Add(Component);
    }
}

void AProceduralTerrainGen::ClearNature()
{
    for (UHierarchicalInstancedStaticMeshComponent* Component : NatureComponents)
        if (IsValid(Component)) Component->DestroyComponent();
    NatureComponents.Empty();
}

void AProceduralTerrainGen::GenerateNature()
{
    ClearNature();
    if (!bGenerateNature) return;

    TD_LOG_INFO_NDSP(TEXT("Generating Nature..."));

    // Everything below derives from Seed, so a level's nature is the same every time that seed is generated
    FRandomStream Stream(Seed + 7919);
    const auto RandomOffset = [&Stream]() -> FVector2D
    { return FVector2D(Stream.FRandRange(0.0f, 10000.0f), Stream.FRandRange(0.0f, 10000.0f)); };
    const FVector2D FoliageNoiseOffset = RandomOffset();
    const FVector2D TreeNoiseOffset = RandomOffset();
    const FVector2D TreeSpeciesNoiseOffset = RandomOffset();
    const FVector2D RockNoiseOffset = RandomOffset();
    const FVector2D GrassNoiseOffset = RandomOffset();

    // Trees, rocks and foliage all keep clear of each other, so share one record of what's already been placed
    FNatureOccupancy Occupied;

    // Trees
    // Species clumps by position, with some random variety mixed in
    if (!Trees.IsEmpty())
    {
        TMap<UStaticMesh*, TArray<FTransform>> Instances;
        ScatterLayer(
            TreeLayer, TreeNoiseOffset, Stream,
            [&](const FVector& Location, FRandomStream& S) -> bool
            {
                const FVector2D Point(Location);
                int32 Index;
                if (S.FRand() < TreeVarietyChance) Index = S.RandHelper(Trees.Num());
                else
                {
                    const float Raw = SampleNatureNoise(Point, TreeSpeciesWavelength, TreeSpeciesNoiseOffset);
                    const float Stretched = FMath::Clamp((Raw - 0.5f) * 2.0f + 0.5f, 0.0f, 0.9999f);
                    Index = FMath::Clamp(FMath::FloorToInt(Stretched * Trees.Num()), 0, Trees.Num() - 1);
                }
                const FProceduralTree& Tree = Trees[Index];
                if (!Tree.TreeMesh) return false;
                const float Scale = RandomScale(S, Tree.MinScale, Tree.MaxScale);
                const float Radius = Tree.OccupiedRadius * Scale;
                if (!Occupied.IsFree(Point, Radius)) return false;
                Occupied.Add(Point, Radius);
                Instances.FindOrAdd(Tree.TreeMesh)
                    .Add(FTransform(FRotator(0.0f, S.FRandRange(0.0f, 360.0f), 0.0f), Location, FVector(Scale)));
                return true;
            });
        CommitNatureInstances(Instances, TreeLayer, true);
    }

    if (!Rocks.IsEmpty())
    {
        TMap<UStaticMesh*, TArray<FTransform>> Instances;
        ScatterLayer(
            RockLayer, RockNoiseOffset, Stream,
            [&](const FVector& Location, FRandomStream& S) -> bool
            {
                const FProceduralRock& Rock = Rocks[S.RandHelper(Rocks.Num())];
                if (!Rock.RockMesh) return false;
                const float Scale = RandomScale(S, Rock.MinScale, Rock.MaxScale);
                const FVector2D Point(Location);
                const float Radius = Rock.OccupiedRadius * Scale;
                if (!Occupied.IsFree(Point, Radius)) return false;
                Occupied.Add(Point, Radius);
                Instances.FindOrAdd(Rock.RockMesh)
                    .Add(FTransform(FRotator(0.0f, S.FRandRange(0.0f, 360.0f), 0.0f), Location, FVector(Scale)));
                return true;
            });
        CommitNatureInstances(Instances, RockLayer, true);
    }

    if (!Foliage.IsEmpty())
    {
        TMap<UStaticMesh*, TArray<FTransform>> Instances;
        ScatterLayer(
            FoliageLayer, FoliageNoiseOffset, Stream,
            [&](const FVector& Location, FRandomStream& S) -> bool
            {
                const FProceduralFoliage& Plant = Foliage[S.RandHelper(Foliage.Num())];
                // Rarer foliage is rejected more often, so it ends up as the occasional one in a patch
                if (!Plant.FoliageMesh || S.FRand() >= Plant.SpawnChance) return false;
                const float Scale = RandomScale(S, Plant.MinScale, Plant.MaxScale);
                const FVector2D Point(Location);
                const float Radius = Plant.OccupiedRadius * Scale;
                if (!Occupied.IsFree(Point, Radius)) return false;
                Occupied.Add(Point, Radius);
                Instances.FindOrAdd(Plant.FoliageMesh)
                    .Add(FTransform(FRotator(0.0f, S.FRandRange(0.0f, 360.0f), 0.0f), Location, FVector(Scale)));
                return true;
            });
        CommitNatureInstances(Instances, FoliageLayer, false);
    }

    // Grass overlaps freely (and grows around trunks and rocks), so it doesn't touch the occupancy record
    if (Grass.GrassMesh)
    {
        TMap<UStaticMesh*, TArray<FTransform>> Instances;
        ScatterLayer(
            GrassLayer, GrassNoiseOffset, Stream,
            [&](const FVector& Location, FRandomStream& S) -> bool
            {
                const float Scale = RandomScale(S, Grass.MinScale, Grass.MaxScale);
                Instances.FindOrAdd(Grass.GrassMesh)
                    .Add(FTransform(FRotator(0.0f, S.FRandRange(0.0f, 360.0f), 0.0f), Location, FVector(Scale)));
                return true;
            });
        CommitNatureInstances(Instances, GrassLayer, false);
    }

    TD_LOG_INFO_NDSP(TEXT("Generated Nature"));
}
