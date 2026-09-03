#include "TowerDefenceGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "ProceduralGen/ProceduralTerrainGen.h"
#include "Settings/GameSettingsSubsystem.h"
#include "TowerDefencePawns/AI/TowerDefencePawnAIControllerFactory.h"
#include "TowerDefencePawns/Attackers/Attacker.h"
#include "TowerDefencePawns/TowerDefencePawn.h"
#include "TowerDefencePawns/TowerDefencePawnFactory.h"

void ATowerDefenceGameMode::BeginPlay()
{
    Super::BeginPlay();
    SUBSCRIBE_TO_EVENTS();
}

void ATowerDefenceGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UNSUBSCRIBE_FROM_EVENTS();
    Super::EndPlay(EndPlayReason);
}

void ATowerDefenceGameMode::StartPlay()
{
    TerrainGen = Cast<AProceduralTerrainGen>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralTerrainGen::StaticClass()));
    // Will these even call?
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("Round")), CurrentWave);
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("WaveState")), FName(TEXT("AwaitingStart")));
    BroadcastEnemyCount();
    FGenericTeamId::SetAttitudeSolver(&ATowerDefenceGameMode::GetAttitude);
}

void ATowerDefenceGameMode::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (!EVENT_MATCHES(TEXT("DeathEvent"), 1) || !PARAMS_ARE_VALID || !PARAMS_ARE_CORRECT_TYPES(ATowerDefencePawn*))
        return;
    ATowerDefencePawn* const* DeadPawnPtr = Params[0].Get<ATowerDefencePawn*>();
    if (!DeadPawnPtr) return;
    if (AAttacker* Enemy = Cast<AAttacker>(*DeadPawnPtr)) HandleEnemyDeath(Enemy);
}

void ATowerDefenceGameMode::StartNextWave()
{
    if (bWaveInProgress || !IsValid(TerrainGen) || EnemyClasses.IsEmpty()) return;
    ++CurrentWave;
    EnemiesLeftToSpawnThisWave = GetEnemyCountForWave(CurrentWave);
    EnemiesAliveThisWave = 0;
    bWaveInProgress = true;
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("Round")), CurrentWave);
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("WaveState")), FName(TEXT("InProgress")));
    BroadcastEnemyCount();
    SpawnBurst();
}

ETeamAttitude::Type ATowerDefenceGameMode::GetAttitude(FGenericTeamId TeamA, FGenericTeamId TeamB)
{
    return TeamA == TeamB ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
}

void ATowerDefenceGameMode::SpawnBurst()
{
    const int32 NumToSpawn = FMath::Min(EnemiesPerBurst, EnemiesLeftToSpawnThisWave);
    for (int32 i = 0; i < NumToSpawn; ++i) SpawnEnemyOnRandomPath();
    EnemiesLeftToSpawnThisWave -= NumToSpawn;
    BroadcastEnemyCount();
    if (EnemiesLeftToSpawnThisWave > 0)
        GetWorldTimerManager().SetTimer(SpawnBurstTimerHandle, this, &ATowerDefenceGameMode::SpawnBurst, BurstInterval,
                                        false);
}

void ATowerDefenceGameMode::SpawnEnemyOnRandomPath()
{
    const TArray<FTerrainPath>& Paths = TerrainGen->GetPaths();
    if (Paths.IsEmpty() || EnemyClasses.IsEmpty()) return;
    const FTerrainPath& Path = Paths[FMath::RandHelper(Paths.Num())];
    if (Path.Points.IsEmpty()) return;
    const TSubclassOf<AAttacker> EnemyClass = EnemyClasses[FMath::RandHelper(EnemyClasses.Num())];
    if (!EnemyClass) return;
    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS)
    {
        AAttacker* Enemy = Cast<AAttacker>(CREATE_PAWN(EnemyClass, FTransform(Path.Points[0])));
        if (!IsValid(Enemy)) return;
        Enemy->SetPathPoints(Path.Points);
        if (Enemy->UseAIController()) POSSESS_TOWER_DEFENCE_PAWN(Enemy);
        Enemy->SetPawnActive(true);
        ++EnemiesAliveThisWave;
    }
}

void ATowerDefenceGameMode::HandleEnemyDeath(AAttacker* Enemy)
{
    if (!IsValid(Enemy)) return;

    const int32 Reward = Enemy->GetCurrencyOnDeath();
    Enemy->OnDeath(
        [this, Enemy, Reward]() -> void
        {
            if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS)
            {
                if (Enemy->UseAIController()) UNPOSSESS_TOWER_DEFENCE_PAWN(Enemy);
                Enemy->SetPawnActive(false);
                DESTROY_PAWN(Enemy);

                EnemiesAliveThisWave = FMath::Max(0, EnemiesAliveThisWave - 1);
                if (Reward > 0) BROADCAST_EVENT(TEXT("MoneyEarnedEvent"), Reward);
                BroadcastEnemyCount();

                CheckWaveComplete();
            }
        });
}

void ATowerDefenceGameMode::CheckWaveComplete()
{
    if (!bWaveInProgress || EnemiesLeftToSpawnThisWave > 0 || EnemiesAliveThisWave > 0) return;
    bWaveInProgress = false;
    const int32 Reward = GetGoldRewardForWave(CurrentWave);
    if (Reward > 0) BROADCAST_EVENT(TEXT("MoneyEarnedEvent"), Reward);
    if (const UGameInstance* GameInstance = GetGameInstance())
    {
        if (const UGameSettingsSubsystem* Settings = GameInstance->GetSubsystem<UGameSettingsSubsystem>())
        {
            if (Settings->IsAutoPlay())
            {
                StartNextWave();
                return;
            }
        }
    }
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("WaveState")), FName(TEXT("AwaitingStart")));
}

int32 ATowerDefenceGameMode::GetEnemyCountForWave(int32 Wave) const
{
    return BaseEnemiesPerWave + EnemiesPerWaveGrowth * FMath::Max(0, Wave - 1);
}

int32 ATowerDefenceGameMode::GetGoldRewardForWave(int32 Wave) const
{
    return BaseWaveClearReward + WaveClearRewardGrowth * FMath::Max(0, Wave - 1);
}

void ATowerDefenceGameMode::BroadcastEnemyCount() const
{
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("EnemyCount")),
                    EnemiesLeftToSpawnThisWave + EnemiesAliveThisWave);
}
