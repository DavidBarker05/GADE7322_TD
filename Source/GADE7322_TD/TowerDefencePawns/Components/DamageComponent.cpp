#include "TowerDefencePawns/Components/DamageComponent.h"

#include "TowerDefencePawns/Components/HealthComponent.h"

void UDamageComponent::DamageOther(UHealthComponent* HealthComponent)
{
    if (HealthComponent && IsValid(HealthComponent)) HealthComponent->TakeDamage(Damage);
}
