#pragma once

#include "CoreMinimal.h"

#include "TowerDefencePawns/Defenders/Defender.h"

#include "Ballista.generated.h"

struct FAIStimulus;
class ABallistaBolt;
class UAIPerceptionComponent;
class UAISenseConfig_Proximity;
class UStaticMeshComponent;

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

protected:
    virtual void DoOnSetActive(bool bActive) override;

    virtual void DoUpdatePerceptionOnTeamChange() override;

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Hertz"))
    float TargetUpdateFrequency = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Degrees"))
    float RotationSpeed = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
    TSubclassOf<ABallistaBolt> BoltClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float ProjectileSpeed = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float ProjectileGravityScale = 1.0f;

    float TimeSinceLastTargetUpdate = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
    TArray<ATowerDefencePawn*> VisiblePawns;

    FTimerHandle AttackTimerHandle;

    FRotator DefaultPivotRotation = FRotator::ZeroRotator;
};
