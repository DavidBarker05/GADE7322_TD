#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "BallistaBolt.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class GADE7322_TD_API ABallistaBolt : public AActor
{
    GENERATED_BODY()

public:
    ABallistaBolt();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    const UBoxComponent* GetBoxCollision() const { return BoxCollision; }
    UBoxComponent* GetBoxCollision() { return BoxCollision; }

    const UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
    UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

    const UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
    UStaticMeshComponent* GetStaticMeshComponent() { return StaticMeshComponent; }

    int32 GetDamage() const { return Damage; }
    ABallistaBolt& SetDamage(int32 InDamage)
    {
        if (InDamage > 0) Damage = InDamage;
        return *this;
    }

    const USceneComponent* GetTarget() const { return Target; }
    USceneComponent* GetTarget() { return Target; }

    ABallistaBolt& SetTarget(USceneComponent* InTarget);

    UFUNCTION(BlueprintCallable)
    void Fire(USceneComponent* InTarget, int32 InDamage, const FVector& LaunchVelocity);

private:
    void ReturnToPool();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UBoxComponent* BoxCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(BlueprintReadWrite, Category = "Ballista", meta = (AllowPrivateAccess = true))
    USceneComponent* Target;

    UPROPERTY(BlueprintReadWrite, Category = "Ballista", meta = (AllowPrivateAccess = true, ClampMin = 1))
    int32 Damage = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float MaxLifetime = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballista",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float HitRadius = 50.0f;

    FTimerHandle LifespanHandle;
};
