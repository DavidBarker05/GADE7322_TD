#include "TowerDefencePawns/Weapon.h"

AWeapon::AWeapon()
{
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
    WeaponMesh->SetupAttachment(RootComponent);
    WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
    bMeshTicks = WeaponMesh && WeaponMesh->IsComponentTickEnabled();
}

bool AWeapon::AttachToSkeleton(USkeletalMeshComponent* Skeleton, EWeaponHand Hand)
{
    // This is basically the only way to do something close to a switch expression in C++ btw
    // It's just a lambda function that I create and call in place
    const FName SocketName = [&Hand]() -> const FName
    {
        switch (Hand)
        {
            case EWeaponHand::Right:
                return FName("weapon_R");
            case EWeaponHand::Left:
                return FName("weapon_L");
            default:
                return {};
        }
    }();
    if (SocketName.IsNone() || !Skeleton || !Skeleton->GetSocketByName(SocketName)) return false;
    this->AttachToComponent(Skeleton, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
    return true;
}
