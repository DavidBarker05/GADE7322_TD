#pragma once

#include "TDCore.h"

#include "TowerDefencePawns/Attackers/Attacker.h"

#include "Golem.generated.h"

class AGolemArm;
class AGolemBody;
class AGolemLeg;
class UChildActorComponent;

UENUM(BlueprintType)
enum class EGolemPartStrength : uint8
{
    // Only ever used as a value if something went wrong
    Unknown UMETA(DisplayName = "Unknown"),
    Weak UMETA(DisplayName = "Weak"),
    Average UMETA(DisplayName = "Average"),
    Strong UMETA(DisplayName = "Strong")
};

// Lots of code, idk if there was a better way to do this but oh well it works :/

UCLASS(Abstract)
class GADE7322_TD_API AGolem : public AAttacker
{
    GENERATED_BODY()

public:
    AGolem();

    virtual void BeginPlay() override;

    const ATowerDefencePawn* GetAttackTarget() const { return CurrentAttackTarget; }
    ATowerDefencePawn* GetAttackTarget() { return CurrentAttackTarget; }

    AGolem& SetAttackTarget(ATowerDefencePawn* Target)
    {
        CurrentAttackTarget = Target;
        return *this;
    }

    float GetAttackRadius() const { return AttackRadius; }

    float GetAttackCooldown() const { return AttackCooldown; }

    static EGolemPartStrength GetRandomPartStrength();

    const UStaticMeshComponent* GetHead() const { return Head; }
    UStaticMeshComponent* GetHead() { return Head; }

    const AGolemArm* GetRightArm(EGolemPartStrength Strength) const;
    AGolemArm* GetRightArm(EGolemPartStrength Strength);
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Right", meta = (DisplayName = "Get Right Arm"))
    AGolemArm* GetRightArm_Blueprint(EGolemPartStrength Strength) const;

    const AGolemArm* GetCurrentRightArm() const { return CurrentRightArm; }
    AGolemArm* GetCurrentRightArm() { return CurrentRightArm; }

    const AGolemArm* GetLeftArm(EGolemPartStrength Strength) const;
    AGolemArm* GetLeftArm(EGolemPartStrength Strength);
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Left", meta = (DisplayName = "Get Left Arm"))
    AGolemArm* GetLeftArm_Blueprint(EGolemPartStrength Strength) const;

    const AGolemArm* GetCurrentLeftArm() const { return CurrentLeftArm; }
    AGolemArm* GetCurrentLeftArm() { return CurrentLeftArm; }

    const AGolemBody* GetBody(EGolemPartStrength Strength) const;
    AGolemBody* GetBody(EGolemPartStrength Strength);
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Body", meta = (DisplayName = "Get Body"))
    AGolemBody* GetBody_Blueprint(EGolemPartStrength Strength) const;

    const AGolemBody* GetCurrentBody() const { return CurrentBody; }
    AGolemBody* GetCurrentBody() { return CurrentBody; }

    const AGolemLeg* GetRightLeg(EGolemPartStrength Strength) const;
    AGolemLeg* GetRightLeg(EGolemPartStrength Strength);
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Right", meta = (DisplayName = "Get Right Leg"))
    AGolemLeg* GetRightLeg_Blueprint(EGolemPartStrength Strength) const;

    const AGolemLeg* GetCurrentRightLeg() const { return CurrentRightLeg; }
    AGolemLeg* GetCurrentRightLeg() { return CurrentRightLeg; }

    const AGolemLeg* GetLeftLeg(EGolemPartStrength Strength) const;
    AGolemLeg* GetLeftLeg(EGolemPartStrength Strength);
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Left", meta = (DisplayName = "Get Left Leg"))
    AGolemLeg* GetLeftLeg_Blueprint(EGolemPartStrength Strength) const;

    const AGolemLeg* GetCurrentLeftLeg() const { return CurrentLeftLeg; }
    AGolemLeg* GetCurrentLeftLeg() { return CurrentLeftLeg; }

protected:
    virtual void DoOnSetActive(bool bActive) override;

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Arm|Right")
    void SetCurrentRightArm(AGolemArm* Arm);

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Arm|Left")
    void SetCurrentLeftArm(AGolemArm* Arm);

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Body")
    void SetCurrentBody(AGolemBody* Body);

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Leg|Right")
    void SetCurrentRightLeg(AGolemLeg* Leg);

    UFUNCTION(BlueprintCallable, Category = "Golem|Part|Leg|Left")
    void SetCurrentLeftLeg(AGolemLeg* Leg);

    const AGolemArm* GetWeakRightArm() const;
    AGolemArm* GetWeakRightArm();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Right", meta = (DisplayName = "Get Weak Right Arm"))
    AGolemArm* GetWeakRightArm_Blueprint() const;

    const AGolemArm* GetAverageRightArm() const;
    AGolemArm* GetAverageRightArm();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Right", meta = (DisplayName = "Get Average Right Arm"))
    AGolemArm* GetAverageRightArm_Blueprint() const;

    const AGolemArm* GetStrongRightArm() const;
    AGolemArm* GetStrongRightArm();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Right", meta = (DisplayName = "Get Strong Right Arm"))
    AGolemArm* GetStrongRightArm_Blueprint() const;

    const AGolemArm* GetWeakLeftArm() const;
    AGolemArm* GetWeakLeftArm();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Left", meta = (DisplayName = "Get Weak Left Arm"))
    AGolemArm* GetWeakLeftArm_Blueprint() const;

    const AGolemArm* GetAverageLeftArm() const;
    AGolemArm* GetAverageLeftArm();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Left", meta = (DisplayName = "Get Average Left Arm"))
    AGolemArm* GetAverageLeftArm_Blueprint() const;

    const AGolemArm* GetStrongLeftArm() const;
    AGolemArm* GetStrongLeftArm();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Arm|Left", meta = (DisplayName = "Get Strong Left Arm"))
    AGolemArm* GetStrongLeftArm_Blueprint() const;

    const AGolemBody* GetWeakBody() const;
    AGolemBody* GetWeakBody();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Body", meta = (DisplayName = "Get Weak Body"))
    AGolemBody* GetWeakBody_Blueprint() const;

    const AGolemBody* GetAverageBody() const;
    AGolemBody* GetAverageBody();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Body", meta = (DisplayName = "Get Average Body"))
    AGolemBody* GetAverageBody_Blueprint() const;

    const AGolemBody* GetStrongBody() const;
    AGolemBody* GetStrongBody();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Body", meta = (DisplayName = "Get Strong Body"))
    AGolemBody* GetStrongBody_Blueprint() const;

    const AGolemLeg* GetWeakRightLeg() const;
    AGolemLeg* GetWeakRightLeg();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Right", meta = (DisplayName = "Get Weak Right Leg"))
    AGolemLeg* GetWeakRightLeg_Blueprint() const;

    const AGolemLeg* GetAverageRightLeg() const;
    AGolemLeg* GetAverageRightLeg();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Right", meta = (DisplayName = "Get Average Right Leg"))
    AGolemLeg* GetAverageRightLeg_Blueprint() const;

    const AGolemLeg* GetStrongRightLeg() const;
    AGolemLeg* GetStrongRightLeg();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Right", meta = (DisplayName = "Get Strong Right Leg"))
    AGolemLeg* GetStrongRightLeg_Blueprint() const;

    const AGolemLeg* GetWeakLeftLeg() const;
    AGolemLeg* GetWeakLeftLeg();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Left", meta = (DisplayName = "Get Weak Left Leg"))
    AGolemLeg* GetWeakLeftLeg_Blueprint() const;

    const AGolemLeg* GetAverageLeftLeg() const;
    AGolemLeg* GetAverageLeftLeg();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Left", meta = (DisplayName = "Get Average Left Leg"))
    AGolemLeg* GetAverageLeftLeg_Blueprint() const;

    const AGolemLeg* GetStrongLeftLeg() const;
    AGolemLeg* GetStrongLeftLeg();
    UFUNCTION(BlueprintPure, Category = "Golem|Part|Leg|Left", meta = (DisplayName = "Get Strong Left Leg"))
    AGolemLeg* GetStrongLeftLeg_Blueprint() const;

    UPROPERTY(BlueprintReadOnly, Category = "Golem|Part|Arm|Right", meta = (AllowPrivateAccess = true))
    AGolemArm* CurrentRightArm = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Golem|Part|Arm|Left", meta = (AllowPrivateAccess = true))
    AGolemArm* CurrentLeftArm = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Golem|Part|Body", meta = (AllowPrivateAccess = true))
    AGolemBody* CurrentBody = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Golem|Part|Leg|Right", meta = (AllowPrivateAccess = true))
    AGolemLeg* CurrentRightLeg = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Golem|Part|Leg|Left", meta = (AllowPrivateAccess = true))
    AGolemLeg* CurrentLeftLeg = nullptr;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
    ATowerDefencePawn* CurrentAttackTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Centimeters"))
    float AttackRadius = 125.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0, Units = "Seconds"))
    float AttackCooldown = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Golem|Part|Head",
              meta = (AllowPrivateAccess = true))
    UStaticMeshComponent* Head;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Arm|Right", meta = (AllowPrivateAccess = true))
    UChildActorComponent* WeakRightArmComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Arm|Right", meta = (AllowPrivateAccess = true))
    UChildActorComponent* AverageRightArmComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Arm|Right", meta = (AllowPrivateAccess = true))
    UChildActorComponent* StrongRightArmComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Arm|Left", meta = (AllowPrivateAccess = true))
    UChildActorComponent* WeakLeftArmComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Arm|Left", meta = (AllowPrivateAccess = true))
    UChildActorComponent* AverageLeftArmComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Arm|Left", meta = (AllowPrivateAccess = true))
    UChildActorComponent* StrongLeftArmComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Body", meta = (AllowPrivateAccess = true))
    UChildActorComponent* WeakBodyComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Body", meta = (AllowPrivateAccess = true))
    UChildActorComponent* AverageBodyComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Body", meta = (AllowPrivateAccess = true))
    UChildActorComponent* StrongBodyComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Leg|Right", meta = (AllowPrivateAccess = true))
    UChildActorComponent* WeakRightLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Leg|Right", meta = (AllowPrivateAccess = true))
    UChildActorComponent* AverageRightLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Leg|Right", meta = (AllowPrivateAccess = true))
    UChildActorComponent* StrongRightLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Leg|Left", meta = (AllowPrivateAccess = true))
    UChildActorComponent* WeakLeftLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Leg|Left", meta = (AllowPrivateAccess = true))
    UChildActorComponent* AverageLeftLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components|Golem|Part|Leg|Left", meta = (AllowPrivateAccess = true))
    UChildActorComponent* StrongLeftLegComponent;
};
