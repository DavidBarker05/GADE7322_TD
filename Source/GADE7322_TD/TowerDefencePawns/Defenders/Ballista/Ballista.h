#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Defenders/Defender.h"

#include "Ballista.generated.h"

struct FAIStimulus;
class AProjectile;
class UAIPerceptionComponent;
class UAISenseConfig_Proximity;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FBallistaProjectile
{
    GENERATED_BODY()

    TSubclassOf<AProjectile> GetProjectileClass() const { return ProjectileClass; }

    float GetProjectileSpeed() const { return ProjectileSpeed; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "cm/s"))
    float ProjectileSpeed = 0.0f;
};

USTRUCT(BlueprintType)
struct FBallistaLevel
{
    GENERATED_BODY()

    EAITeam GetTeam() const { return AITeam; }

    float GetAttackRadius() const { return AttackRadius; }

    float GetAttackCooldown() const { return AttackCooldown; }

    const FBallistaProjectile& GetProjectile() const { return BallistaProjectile; }
    FBallistaProjectile& GetProjectile() { return BallistaProjectile; }

    bool DamageGroundTroops() const { return bDamageGroundTroops; }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades|AI",
              meta = (AllowPrivateAccess = true))
    EAITeam AITeam = EAITeam::RangedDefender;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades|AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades|Projectile",
              meta = (AllowPrivateAccess = true))
    FBallistaProjectile BallistaProjectile;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades|Projectile",
              meta = (AllowPrivateAccess = true))
    bool bDamageGroundTroops = false;
};

UCLASS(Abstract)
class GADE7322_TD_API ABallista : public ADefender
{
    GENERATED_BODY()

public:
    ABallista();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    virtual void StartAttack() override;

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }

    ATowerDefencePawn* GetAttackTarget() { return CurrentAttackTarget; }

    ABallista& SetAttackTarget(ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    virtual float GetDetectionRadius() const override { return AttackRadius; }
    virtual float GetPrimaryRadius() const override { return AttackRadius; }

    FBallistaLevel GetBallistaBaseLevel() const { return BallistaBaseLevel; }
    FBallistaLevel GetBallistaFirstUpgrade() const { return BallistaFirstUpgrade; }
    FBallistaLevel GetBallistaSecondUpgrade() const { return BallistaSecondUpgrade; }

    UFUNCTION(BlueprintPure)
    FBallistaLevel GetBallistaCurrentLevel() const;

protected:
    virtual void DoOnSetActive(bool bActive) override;

    virtual void DoUpdatePerceptionOnTeamChange() override;

    virtual void DoOnLevelChanged(int32 Level) override;

private:
    static float GetAngleToFireProjectile(const UObject* WorldContextObject, FVector LaunchLocation,
                                          FVector TargetLocation, float InitialVelocity, float ProjectileGravityScale);
    // ^ float because only need 1 angle

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    void UpdateAttackTarget();

    void UpdatePivotRotation(float DeltaTime);

    void FireProjectile();

    FVector ComputeLaunchVelocity(const FVector& LaunchLocation, const FVector& TargetLocation) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* StandMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USceneComponent* PivotPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* BallistaMesh;

    // Where bolts actually spawn/launch from
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USceneComponent* MuzzlePoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UAIPerceptionComponent* PerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    UAISenseConfig_Proximity* ProximityConfig;

    UPROPERTY(BlueprintReadWrite, Category = "TD Pawn", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades", meta = (AllowPrivateAccess = true))
    FBallistaLevel BallistaBaseLevel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades", meta = (AllowPrivateAccess = true))
    FBallistaLevel BallistaFirstUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista|Upgrades", meta = (AllowPrivateAccess = true))
    FBallistaLevel BallistaSecondUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, ClampMax = 100.0, UIMax = 100.0,
                      Units = "%"))
    float GroundTroopDamageReduction = 20.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile|Ballista|Cannon",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float ExplosionRadius = 250.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, Units = "Centimeters"))
    float AttackRadius;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, Units = "Seconds"))
    float AttackCooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Hertz"))
    float TargetUpdateFrequency = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Degrees"))
    float RotationSpeed = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float ProjectileGravityScale = 1.0f;

    float TimeSinceLastTargetUpdate = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    TArray<ATowerDefencePawn*> VisiblePawns;

    FTimerHandle AttackTimerHandle;

    FRotator DefaultPivotRotation = FRotator::ZeroRotator;
};
