// ReSharper disable CppParameterMayBeConst
#include "ProceduralGen/ProceduralTerrainGen.h"

#include "DrawDebugHelpers.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "TowerDefencePawns/Defenders/DefenderSpot.h"

AProceduralTerrainGen::AProceduralTerrainGen()
{
    PrimaryActorTick.bCanEverTick = false;

    TerrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
    SetRootComponent(TerrainMesh);
    TerrainMesh->SetMobility(EComponentMobility::Static); // Never moves after BeginPlay, lets the engine optimise it
    TerrainMesh->bUseAsyncCooking = true; // Collision cooking for a mesh this size would be slow if not async
}

void AProceduralTerrainGen::BeginPlay()
{
    Super::BeginPlay();
    GeneratePaths();
    GenerateTerrain();
    GenerateDefenderSpots();
}

void AProceduralTerrainGen::GeneratePaths()
{
    if (bRandomSeedEachGame) Seed = FMath::Rand();

    const FRandomStream Stream(Seed);

    Paths.Empty(NumPaths); // Clears any previous roll and pre-allocates for the new one

    // ReSharper disable once CppTooWideScopeInitStatement
    const TArray<float> EntryAngles = GenerateEntryAngles(Stream);
    for (const float Angle : EntryAngles) Paths.Add(BuildPath(Angle, Stream));

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
    return Path;
}

TArray<FVector> AProceduralTerrainGen::BuildPathControlPoints(float EntryAngleDegrees, const FRandomStream& Stream) const
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

        // Tapered the same as the wander offset above: every path still has to meet exactly at the
        // tower and start exactly at its own (already angle-separated) edge spawn, so this can't be
        // allowed to push those two ends around - only the middle of the path is free to steer away.
        Point += ComputePathSeparationOffset(Point) * Taper;

        ControlPoints.Add(Point);
    }

    return ControlPoints;
}

// Checks Point against every already-built path in Paths (this path's own points aren't in there
// yet) and returns how far it should be nudged away to keep at least MinPathSeparation clear of
// each one's corridor edge. Only the single closest point on each other path counts, not every
// point on it - otherwise being near one small arc of a densely-sampled spline would rack up dozens
// of near-duplicate pushes and wildly overshoot.
FVector AProceduralTerrainGen::ComputePathSeparationOffset(const FVector& Point) const
{
    FVector TotalPush = FVector::ZeroVector;

    for (const FTerrainPath& OtherPath : Paths)
    {
        float ClosestEdgeDistance = TNumericLimits<float>::Max();
        FVector ClosestPoint = FVector::ZeroVector;

        for (const FVector& OtherPoint : OtherPath.Points)
        {
            const float EdgeDistance = FVector::Dist2D(Point, OtherPoint) - PathWidth - OtherPath.Width;
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

// Catmull-Rom spline through the coarse control points. Connecting those points with straight
// lines is what made the path look like sharp triangular zigzags - it'd wander out to a point then
// cut straight back, with no curvature carrying the direction through. A spline instead flows
// smoothly through every control point, turning those same wander offsets into rounded curves.
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

        // Catmull-Rom needs a point before P1 and after P2 to know which way the curve is heading
        // at the ends of this segment. At the very start/end of the path there's no real neighbour
        // to use, so extrapolate one by continuing the existing direction - duplicating P1/P2
        // instead would give a zero-length chord there, which is a divide-by-zero below.
        const FVector P0 = i > 0 ? ControlPoints[i - 1] : P1 * 2.0f - P2;
        const FVector P3 = i < LastIndex - 1 ? ControlPoints[i + 2] : P2 * 2.0f - P1;

        // Centripetal parameterisation (sqrt of chord length): keeps the curve from looping or
        // cusping where control points end up unevenly spaced, which uniform (0,1,2,3) spacing can't.
        const float T0 = 0.0f;
        const float T1 = T0 + FMath::Sqrt(FVector::Dist(P0, P1));
        const float T2 = T1 + FMath::Sqrt(FVector::Dist(P1, P2));
        const float T3 = T2 + FMath::Sqrt(FVector::Dist(P2, P3));

        // Every segment contributes its points up to but not including P2, except the very last
        // segment, which also includes its own endpoint since there's no following segment to add it
        const int32 NumSteps = (i == LastIndex - 1) ? SplineSubdivisions + 1 : SplineSubdivisions;
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
    TerrainMesh->ClearAllMeshSections();

    // Extend the mesh past TerrainRadius so path wander/width/blend never runs off the edge of the grid.
    const float HalfExtent = TerrainRadius + PathWanderAmount + PathWidth + PathFlatZoneWidth +
                             FMath::Max(PathHeightBlendWidth, PathTextureBlendWidth);
    const int32 NumCells = FMath::Max(1, FMath::CeilToInt(HalfExtent * 2.0f / CellSize)); // At least 1 cell
    const int32 VertsPerSide = NumCells + 1; // N cells needs N+1 verts per row/column

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    // R channel is the path/terrain texture blend mask: 0 inside a path corridor, ramping up to 1 by
    // PathTextureBlendWidth out - independent of the (usually wider) PathHeightBlendWidth the height
    // above already blended over. G and B are set the same so a material can read whichever channel
    // is convenient. In the material graph: VertexColor -> Lerp(PathLook, TerrainLook, Alpha =
    // VertexColor.R) -> Base Color (and Roughness/Normal etc. the same way if the looks differ there too).
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
    TerrainMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    if (TerrainMaterial) TerrainMesh->SetMaterial(0, TerrainMaterial);
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
    const float EdgeDistance = DistanceToNearestPathEdge(WorldXY); // Negative = inside a path, shared by both blends below

    // Subtracting PathFlatZoneWidth first means the ramp doesn't even begin until past that buffer -
    // SmoothStep clamps negative input to 0, so everything inside the flat zone stays pinned flat
    // instead of already being partway up the ramp to full noise amplitude.
    const float HeightAlpha = FMath::SmoothStep(0.0f, PathHeightBlendWidth, EdgeDistance - PathFlatZoneWidth);
    OutHeight = FMath::Lerp(PathHeight, NoiseHeight, HeightAlpha);

    OutTextureBlendAlpha = FMath::SmoothStep(0.0f, PathTextureBlendWidth, EdgeDistance);
}

// Fractal Brownian Motion: stack several Perlin samples on top of each other, each one higher
// frequency and lower amplitude than the last, so the terrain gets both big rolling hills (low
// octaves) and small bumps (high octaves) instead of looking like one uniform wave
float AProceduralTerrainGen::SampleNoiseHeight(const FVector2D& WorldXY) const
{
    float Total = 0.0f;
    float AmplitudeSum =
        0.0f; // Tracks the max possible Total so we can normalise back to -1..1 regardless of NoiseOctaves
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

// Checked against every path, not just the nearest one geographically, because two paths can run
// close together (e.g. near the tower) and a point can be outside path A's corridor but still
// inside path B's
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

void AProceduralTerrainGen::DrawDebugForPaths() const
{
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
}

void AProceduralTerrainGen::GenerateDefenderSpots()
{
    for (ADefenderSpot* Spot : DefenderSpots)
    {
        if (IsValid(Spot)) Spot->Destroy();
    }
    DefenderSpots.Empty();

    if (!DefenderSpotClass) return;

    TArray<FVector> PlacedLocations;

    for (const auto& [Points, Width] : Paths)
    {
        // Stagger the first spot half a spacing in so it doesn't sit right on the edge spawn point
        float DistanceUntilNextSpot = DefenderSpotSpacing * 0.5f;

        // Walk the path's polyline segment by segment, treating it as one continuous line (arc
        // length), so spots end up evenly spaced along the whole path rather than reset to 0 at
        // every bend
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

                const float SpotDistance = Width + DefenderSpotOffset;
                TrySpawnDefenderSpot(SegPoint + Perpendicular * SpotDistance, PlacedLocations); // One spot on the left
                TrySpawnDefenderSpot(SegPoint - Perpendicular * SpotDistance, PlacedLocations); // One spot on the right

                DistanceUntilNextSpot = DefenderSpotSpacing; // Back to full spacing for the next spot
            }

            DistanceUntilNextSpot -= SegRemaining; // Carry the unused leftover distance into the next segment
        }
    }

    if (bDrawDebugDefenderSpots) DrawDebugForDefenderSpots();
}

void AProceduralTerrainGen::TrySpawnDefenderSpot(const FVector& CandidateLocation, TArray<FVector>& PlacedLocations)
{
    const FVector2D CandidateXY(CandidateLocation.X, CandidateLocation.Y);

    // Reject if it would overlap any path's walkable corridor
    if (DistanceToNearestPathEdge(CandidateXY) < 0.0f) return;

    // Reject if it's too close to a spot already placed this call
    for (const FVector& Existing : PlacedLocations)
    {
        if (FVector::DistSquared2D(Existing, CandidateLocation) < FMath::Square(DefenderSpotMinSeparation)) return;
    }

    const FVector SpawnLocation(CandidateLocation.X, CandidateLocation.Y, GetTerrainHeight(CandidateXY));

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (ADefenderSpot* NewSpot =
            GetWorld()->SpawnActor<ADefenderSpot>(DefenderSpotClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams))
    {
        DefenderSpots.Add(NewSpot);
        PlacedLocations.Add(SpawnLocation); // So the next candidate's separation check also sees this one
    }
}

void AProceduralTerrainGen::DrawDebugForDefenderSpots() const
{
    const UWorld* const World = GetWorld();
    if (!World) return;

    for (const ADefenderSpot* Spot : DefenderSpots)
    {
        if (IsValid(Spot))
            DrawDebugSphere(World, Spot->GetActorLocation(), 60.0f, 12, FColor::Cyan, false, DebugDrawDuration, 0,
                            3.0f);
    }
}
