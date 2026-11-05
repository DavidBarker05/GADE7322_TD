#include "TowerDefencePawns/Components/DamageComponent.h"

#include "TowerDefencePawns/Components/HealthComponent.h"

void UDamageComponent::DamageOther(UHealthComponent* HealthComponent)
{
    if (HealthComponent && IsValid(HealthComponent)) HealthComponent->TakeDamage(GetDamage());
}

void UDamageComponent::SetStrengthMultiplier(float Multiplier)
{
    if (Multiplier < 0.0f) return;
    bDoStrengthMultiplier = true;
    StrengthMultiplier = Multiplier;
}

void UDamageComponent::RemoveStrengthMultiplier()
{
    bDoStrengthMultiplier = false;
    StrengthMultiplier = 1.0f;
}

float UDamageComponent::GetDamageMultiplier() const { return DamageBoosts.IsEmpty() ? 1.0f : DamageBoosts.Top(); }

bool UDamageComponent::AddDamageBoost(float Multiplier)
{
    DamageBoosts.Push(Multiplier);
    return DamageBoosts.Num() == 1;
}

bool UDamageComponent::RemoveDamageBoost(float Multiplier)
{
    if (DamageBoosts.IsEmpty()) return false;
    TArray<float>& DamageBoostsContainer = DamageBoosts.GetContainer();
    for (int32 i = 0; i < DamageBoostsContainer.Num(); ++i)
    {
        if (DamageBoostsContainer[i] == Multiplier)
        {
            DamageBoostsContainer.HeapRemoveAt(i, TGreater<float>());
            break;
        }
    }
    return DamageBoosts.IsEmpty();
}

int32 UDamageComponent::GetDamage() const
{
    return DamageBoosts.IsEmpty() ?
               (bDoStrengthMultiplier ? FMath::RoundToInt32(Damage * StrengthMultiplier) : Damage) :
               (bDoStrengthMultiplier ? FMath::RoundToInt32(Damage * StrengthMultiplier * GetDamageMultiplier()) :
                                        FMath::RoundToInt32(Damage * GetDamageMultiplier()));
}

void UDamageComponent::SetDamage(int32 InDamage) { Damage = FMath::Max(InDamage, 0); }
