#include "ProceduralGen/ProceduralTerrainGen.h"

AProceduralTerrainGen::AProceduralTerrainGen() { PrimaryActorTick.bCanEverTick = false; }

void AProceduralTerrainGen::BeginPlay() { Super::BeginPlay(); }

void AProceduralTerrainGen::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
