#pragma once

#include "CoreMinimal.h"

#include "Any/Any.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

class ATowerDefencePawn;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

#ifndef Fire
#define Fire(InTarget, LaunchVelocity, InDamage, ...) FireProjectile(InTarget, LaunchVelocity, InDamage, {__VA_ARGS__})
#endif

UCLASS(Abstract)
class GADE7322_TD_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    AProjectile();
    virtual void Tick(float DeltaTime) override;

    const UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
    UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

    const UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
    UStaticMeshComponent* GetStaticMeshComponent() { return StaticMeshComponent; }

    const USceneComponent* GetTarget() const { return Target; }
    USceneComponent* GetTarget() { return Target; }

    int32 GetDamage() const { return Damage; }

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Fire"))
    virtual void FireProjectile(USceneComponent* InTarget, const FVector& LaunchVelocity, int32 InDamage,
                                const TArray<FAny>& LaunchArguments);

protected:
    virtual void FireWithoutLaunch(USceneComponent* InTarget, int32 InDamage);

    virtual void DamageTarget(ATowerDefencePawn* TargetToDamage);

    virtual void OnReturnToPool() { }

    void Launch(const FVector& LaunchVelocity);

private:
    virtual void ReturnToPool();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USphereComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
    USceneComponent* Target;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
    int32 Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float MaxLifetime = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HitRadius = 50.0f;

    FTimerHandle LifespanHandle;
};
