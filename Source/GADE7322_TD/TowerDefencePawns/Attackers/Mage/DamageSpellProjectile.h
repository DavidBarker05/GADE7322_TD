#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "DamageSpellProjectile.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS(Abstract)
class GADE7322_TD_API ADamageSpellProjectile : public AActor
{
    GENERATED_BODY()

public:
    ADamageSpellProjectile();

    virtual void Tick(float DeltaTime) override;

    const USphereComponent* GetSphereCollider() const { return SphereCollider; }
    USphereComponent* GetSphereCollider() { return SphereCollider; }

    const UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
    UProjectileMovementComponent* GetProjectileMovement() { return ProjectileMovement; }

    const UNiagaraComponent* GetSpellEffect() const { return SpellEffect; }
    UNiagaraComponent* GetSpellEffect() { return SpellEffect; }

    const USceneComponent* GetTarget() const { return Target; }
    USceneComponent* GetTarget() { return Target; }

    int32 GetDamage() const { return Damage; }

    float GetMaxLifetime() const { return MaxLifetime; }

    float GetHitDetectionRadius() const { return HitDetectionRadius; }

    UFUNCTION(BlueprintCallable)
    void Fire(USceneComponent* InTarget, int32 InDamage, const FVector& LaunchVelocity);

private:
    void SetTarget(USceneComponent* InTarget);

    void ReturnToPool();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    USphereComponent* SphereCollider;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UNiagaraComponent* SpellEffect;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
    USceneComponent* Target;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true, ClampMin = 1))
    int32 Damage = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float MaxLifetime = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HitDetectionRadius = 100.0f;

    FTimerHandle LifespanHandle;
};
