#include "TowerDefenceGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "ProceduralGen/ProceduralTerrainGen.h"
#include "Settings/GameSettingsSubsystem.h"
#include "TowerDefencePawns/Attackers/Attacker.h"
#include "TowerDefencePawns/Components/HealthComponent.h"
#include "TowerDefencePawns/Defenders/Defender.h"
#include "TowerDefencePawns/Tower/PlayerTower.h"
#include "TowerDefencePawns/TowerDefencePawn.h"
#include "TowerDefencePawns/TowerDefencePawnFactory.h"
#include "UI/TowerDefence/TowerDefenceHUD.h"

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
    Super::StartPlay(); // <- Don't forget to call this, worst mistake, it makes everything else's BeginPlay happen
    TerrainGen = Cast<AProceduralTerrainGen>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralTerrainGen::StaticClass()));
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("Round")), CurrentWave);
    BROADCAST_EVENT(TEXT("UpdateHUDEvent"), FName(TEXT("WaveState")), FName(TEXT("AwaitingStart")));
    BroadcastEnemyCount();
    FGenericTeamId::SetAttitudeSolver(&ATowerDefenceGameMode::GetAttitude);
}

void ATowerDefenceGameMode::OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params)
{
    if (EVENT_MATCHES(TEXT("DeathEvent"), 1))
    {
        if (!PARAMS_ARE_VALID || !PARAMS_ARE_CORRECT_TYPES(ATowerDefencePawn*)) return;
        ATowerDefencePawn* DeadPawn = *Params[0].Get<ATowerDefencePawn*>();
        if (!DeadPawn) return;
        if (AAttacker* Enemy = Cast<AAttacker>(DeadPawn)) HandleEnemyDeath(Enemy);
        else if (APlayerTower* Tower = Cast<APlayerTower>(DeadPawn)) HandleTowerDeath(Tower);
        else if (ADefender* DeadDefender = Cast<ADefender>(DeadPawn))
        {
            if (NumAliveDefenders > 0) --NumAliveDefenders;
            HealthTotal = FMath::Max(0, HealthTotal - DeadDefender->GetHealthComponent()->GetMaxHealth());
        }
    }
    else if (EVENT_MATCHES(TEXT("DefenderPurchasedEvent"), 1))
    {
        if (!PARAMS_ARE_VALID || !PARAMS_ARE_CORRECT_TYPES(int32)) return;
        const int32 DefenderMaxHealth = *Params[0].Get<int32>();
        ++NumAliveDefenders;
        ++NumTotalDefenders;
        HealthTotal += DefenderMaxHealth;
    }
    else if (EVENT_MATCHES(TEXT("DefenderSoldEvent"), 1))
    {
        if (NumAliveDefenders == 0) return;
        if (!PARAMS_ARE_VALID || !PARAMS_ARE_CORRECT_TYPES(int32)) return;
        const int32 DefenderMaxHealth = *Params[0].Get<int32>();
        --NumAliveDefenders;
        --NumTotalDefenders;
        HealthTotal = FMath::Max(0, HealthTotal - DefenderMaxHealth);
    }
    else if (EVENT_MATCHES(TEXT("LeakEvent"), 0)) ++NumLeakedEnemies;
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
    NumTotalDefenders = NumAliveDefenders;
    NumLeakedEnemies = 0;
    HealthGained = 0;
    HealthLost = 0;
    BroadcastEnemyCount();
    SpawnBurst();
}

ETeamAttitude::Type ATowerDefenceGameMode::GetAttitudeCustom(EAITeam TeamA, EAITeam TeamB)
{
    switch (TeamA)
    {
        case EAITeam::MeleeDefender:
            switch (TeamB)
            {
                case EAITeam::MeleeDefender:
                case EAITeam::RangedDefender:
                case EAITeam::SupportDefender:
                    return ETeamAttitude::Friendly; // Friendly to all defenders
                case EAITeam::MeleeAttacker:
                case EAITeam::SupportAttacker:
                    return ETeamAttitude::Hostile; // Hostile to attackers on ground
                case EAITeam::FlyingAttacker:
                    return ETeamAttitude::Neutral; // Neutral to attackers in air (doesn't attack)
                default:
                    return ETeamAttitude::Neutral; // Neutral by default
            }
        case EAITeam::RangedDefender:
        case EAITeam::SupportDefender:
            switch (TeamB)
            {
                case EAITeam::MeleeDefender:
                case EAITeam::RangedDefender:
                case EAITeam::SupportDefender:
                    return ETeamAttitude::Friendly; // Friendly to all defenders
                case EAITeam::MeleeAttacker:
                case EAITeam::SupportAttacker:
                case EAITeam::FlyingAttacker:
                    return ETeamAttitude::Hostile; // Hostile to all attackers
                default:
                    return ETeamAttitude::Neutral; // Neutral by default
            }
        case EAITeam::MeleeAttacker:
        case EAITeam::FlyingAttacker:
        case EAITeam::SupportAttacker:
            switch (TeamB)
            {
                case EAITeam::MeleeDefender:
                case EAITeam::RangedDefender:
                case EAITeam::SupportDefender:
                    return ETeamAttitude::Hostile; // Hostile to all defenders
                case EAITeam::MeleeAttacker:
                case EAITeam::SupportAttacker:
                case EAITeam::FlyingAttacker:
                    return ETeamAttitude::Friendly; // Friendly to all attackers
                default:
                    return ETeamAttitude::Neutral; // Neutral by default
            }
        default:
            return ETeamAttitude::Neutral; // Neutral by default
    }
}

ETeamAttitude::Type ATowerDefenceGameMode::GetAttitude(FGenericTeamId TeamA, FGenericTeamId TeamB)
{
    const EAITeam AiTeamA = static_cast<EAITeam>(TeamA.GetId());
    const EAITeam AiTeamB = static_cast<EAITeam>(TeamB.GetId());
    return GetAttitudeCustom(AiTeamA, AiTeamB);
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
        FVector SpawnLocation = Path.Points[0];
        const FVector2D Jitter = FMath::RandPointInCircle(75.0f);
        SpawnLocation.X += Jitter.X;
        SpawnLocation.Y += Jitter.Y;
        AAttacker* Enemy = Cast<AAttacker>(CREATE_PAWN(EnemyClass, FTransform(SpawnLocation)));
        if (!IsValid(Enemy)) return;
        Enemy->SetPathPoints(Path.Points);
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
                Enemy->SetPawnActive(false);
                DESTROY_PAWN(Enemy);
                EnemiesAliveThisWave = FMath::Max(0, EnemiesAliveThisWave - 1);
                if (Reward > 0) BROADCAST_EVENT(TEXT("MoneyEarnedEvent"), Reward);
                BroadcastEnemyCount();
                CheckWaveComplete();
            }
        });
}

void ATowerDefenceGameMode::HandleTowerDeath(APlayerTower* Tower)
{
    if (!IsValid(Tower)) return;
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetPause(true);
        if (ATowerDefenceHUD* HUD = Cast<ATowerDefenceHUD>(PC->GetHUD())) HUD->ShowLoseScreen();
    }
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
