#include "TowerDefencePawns/Defenders/Warrior/WarriorShield.h"

#include "TowerDefencePawns/Components/HitFlashComponent.h"
#include "TowerDefencePawns/Components/OutlineComponent.h"

AWarriorShield::AWarriorShield()
{
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Mesh"));
    ShieldMesh->SetupAttachment(RootComponent);
    ShieldMesh->SetCollisionProfileName(TEXT("NoCollision"));
    HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("Hit Flash Component"));
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
    bMeshTicks = ShieldMesh && ShieldMesh->IsComponentTickEnabled();
}

bool AWarriorShield::AttachToSkeleton(USkeletalMeshComponent* Skeleton, EShieldArm Arm)
{
    const FName SocketName = [&Arm]() -> const FName
    {
        switch (Arm)
        {
            case EShieldArm::Right:
                return FName("shield_R");
            case EShieldArm::Left:
                return FName("shield_L");
            default:
                return NAME_None;
        }
    }();
    if (SocketName.IsNone() || !Skeleton || !Skeleton->GetSocketByName(SocketName)) return false;
    this->AttachToComponent(Skeleton, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
    return true;
}
