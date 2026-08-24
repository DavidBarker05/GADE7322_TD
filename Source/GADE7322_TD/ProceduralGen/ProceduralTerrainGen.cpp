#include "ProceduralGen/ProceduralTerrainGen.h"

#include "DrawDebugHelpers.h"

AProceduralTerrainGen::AProceduralTerrainGen() { PrimaryActorTick.bCanEverTick = false; }

void AProceduralTerrainGen::BeginPlay()
{
    Super::BeginPlay();
    GeneratePaths();
}

void AProceduralTerrainGen::GeneratePaths()
{
    if (bRandomSeedEachGame) Seed = FMath::Rand();

    FRandomStream Stream(Seed);

    Paths.Empty(NumPaths);

    const TArray<float> EntryAngles = GenerateEntryAngles(Stream);
    for (const float Angle : EntryAngles) Paths.Add(BuildPath(Angle, Stream));

    if (bDrawDebugPaths) DrawDebugForPaths();
}

TArray<float> AProceduralTerrainGen::GenerateEntryAngles(FRandomStream& Stream) const
{
    TArray<float> Angles;
    Angles.Reserve(NumPaths);

    const float Spacing = 360.0f / NumPaths;
    const float MaxJitter = FMath::Max(0.0f, (Spacing - MinEntryAngleSeparation) * 0.5f);

    for (int32 i = 0; i < NumPaths; ++i)
    {
        const float BaseAngle = i * Spacing;
        const float Jitter = Stream.FRandRange(-MaxJitter, MaxJitter);
        Angles.Add(FMath::Fmod(BaseAngle + Jitter + 360.0f, 360.0f));
    }

    return Angles;
}

FTerrainPath AProceduralTerrainGen::BuildPath(float EntryAngleDegrees, FRandomStream& Stream) const
{
    FTerrainPath Path;
    Path.Width = PathWidth;
    Path.Points.Reserve(PathSegments + 1);

    const float AngleRadians = FMath::DegreesToRadians(EntryAngleDegrees);
    const FVector EnemySpawn(TerrainRadius * FMath::Cos(AngleRadians), TerrainRadius * FMath::Sin(AngleRadians), 0.0f);
    const FVector TowerPos = FVector::ZeroVector;

    const FVector Direction = (TowerPos - EnemySpawn).GetSafeNormal();
    const FVector Perpendicular(-Direction.Y, Direction.X, 0.0f);

    // Offsets this path's noise sample range so different paths don't wander in lockstep
    const float NoiseSeedOffset = Stream.FRandRange(0.0f, 1000.0f);

    for (int32 i = 0; i <= PathSegments; ++i)
    {
        const float Alpha = static_cast<float>(i) / PathSegments;
        const FVector BasePoint = FMath::Lerp(EnemySpawn, TowerPos, Alpha);

        // Tapers to zero at both ends so the path still starts on the edge and ends exactly at the tower
        const float Taper = FMath::Sin(PI * Alpha);
        const float NoiseSample = FMath::PerlinNoise1D(Alpha * PathSegments * PathWanderFrequency + NoiseSeedOffset);

        const FVector Offset = Perpendicular * NoiseSample * PathWanderAmount * Taper;
        Path.Points.Add(BasePoint + Offset);
    }

    return Path;
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
