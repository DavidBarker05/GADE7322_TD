#include "DamageComponent.h"

#include "HealthComponent.h"

UDamageComponent::UDamageComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UDamageComponent::BeginPlay() { Super::BeginPlay(); }

void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDamageComponent::DamageOther(UHealthComponent* HealthComponent)
{
    if (HealthComponent && IsValid(HealthComponent)) HealthComponent->TakeDamage(Damage);
}

int32 UDamageComponent::GetDamage() const { return Damage; }

void UDamageComponent::SetDamage(int32 InDamage) { Damage = FMath::Max(0, InDamage); }
