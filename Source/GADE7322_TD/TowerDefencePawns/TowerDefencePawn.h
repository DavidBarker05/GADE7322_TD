#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GenericTeamAgentInterface.h"

#include "TowerDefencePawn.generated.h"

class ATowerDefencePawnAIController;
class UHealthComponent;
class UDamageComponent;
class UAIPerceptionStimuliSourceComponent;

UENUM(BlueprintType)
enum class EAITeam : uint8
{
    Defender = 0 UMETA(DisplayName = "Defender"),
    Attacker = 1 UMETA(DisplayName = "Attacker"),
};
// ^ will probably add more teams when I add different types

UCLASS(Abstract)
class GADE7322_TD_API ATowerDefencePawn : public APawn,
                                          public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    ATowerDefencePawn();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    // Play like an attack animation, or shoot projectile, etc.
    UFUNCTION(BlueprintCallable)
    virtual void StartAttack() { }

    UFUNCTION(BlueprintCallable)
    virtual void Attack(ATowerDefencePawn* Other);

    // After finish attack animation or whatever, do any end stuff and if it needs to allow attack again do that
    UFUNCTION(BlueprintCallable)
    virtual void EndAttack() { }

    virtual void OnDeath(TFunction<void()>&& Func); // If override for animation don't forget to call delegate

    UFUNCTION(BlueprintCallable)
    virtual void OnDeathComplete();

    const UHealthComponent* GetHealthComponent() const { return HealthComponent; }
    UHealthComponent* GetHealthComponent() { return HealthComponent; }

    const UDamageComponent* GetDamageComponent() const { return DamageComponent; }
    UDamageComponent* GetDamageComponent() { return DamageComponent; }

    const FName& GetPawnDisplayName() const { return PawnDisplayName; }

    ATowerDefencePawn& SetPawnActive(bool bActive);

    bool IsPawnActive() const { return bIsPawnActive; }

    float GetOccupiedRadius() const { return OccupiedRadius; }

    bool CanAttack() const { return bCanAttack; }

    bool UseAIController() const { return bUseAIController; }

    TSubclassOf<ATowerDefencePawnAIController> GetTDAIControllerClass() const { return TDAIControllerClass; }

    EAITeam GetCurrentTeam() const { return CurrentTeam; }

    ATowerDefencePawn& SetCurrentTeam(EAITeam NewTeam)
    {
        CurrentTeam = NewTeam;
        DoUpdatePerceptionOnTeamChange();
        return *this;
    }

    virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(static_cast<uint8>(CurrentTeam)); }

    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override
    {
        CurrentTeam = static_cast<EAITeam>(NewTeamId.GetId());
        DoUpdatePerceptionOnTeamChange();
    }

protected:
    // Stuff like toggling mesh, etc.
    virtual void DoOnSetActive(bool bActive) { }

    virtual void DoUpdatePerceptionOnTeamChange() { }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UDamageComponent* DamageComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
    UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Teams")
    EAITeam CurrentTeam;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name", meta = (AllowPrivateAccess = true))
    FName PawnDisplayName = TEXT("TowerDefencePawn");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Size",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float OccupiedRadius = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = true))
    bool bCanAttack = true;

    UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (DisplayName = "TD AI Controller Class"))
    TSubclassOf<ATowerDefencePawnAIController> TDAIControllerClass;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    bool bUseAIController = false;

    TFunction<void()> DestroyDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TD Pawn", meta = (AllowPrivateAccess = true))
    bool bIsPawnActive = false;
};
