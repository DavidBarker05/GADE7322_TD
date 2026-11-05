#include "TowerDefencePawns/Attackers/ProceduralEnemy/ProceduralParts/GolemCollapse.h"

FGolemCollapse::FSavedAttachment FGolemCollapse::Save(UStaticMeshComponent* Rock)
{
    FSavedAttachment Saved;
    Saved.Rock = Rock;
    Saved.Parent = Rock->GetAttachParent();
    Saved.Relative = Rock->GetRelativeTransform();
    Saved.Socket = Rock->GetAttachSocketName();
    Saved.CollisionChannel = Rock->GetCollisionObjectType();
    return Saved;
}

void FGolemCollapse::SetupCollision(UStaticMeshComponent* Rock)
{
    if (!IsValid(Rock)) return;
    Rock->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Rock->SetCollisionObjectType(ECC_PhysicsBody);
    Rock->SetCollisionResponseToAllChannels(ECR_Ignore);
    Rock->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Rock->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
    Rock->SetCanEverAffectNavigation(false);
    Rock->SetLinearDamping(0.4f);
    Rock->SetAngularDamping(0.8f);
}

void FGolemCollapse::ResetCollision(UStaticMeshComponent* Rock, ECollisionChannel CollisionChannel)
{
    if (!IsValid(Rock)) return;
    Rock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Rock->SetCollisionObjectType(CollisionChannel);
    Rock->SetCollisionResponseToAllChannels(ECR_Ignore);
    Rock->SetCanEverAffectNavigation(false);
    Rock->SetLinearDamping(0.0f);
    Rock->SetAngularDamping(0.0f);
}

void FGolemCollapse::Restore(const FSavedAttachment& Saved)
{
    UStaticMeshComponent* Rock = Saved.Rock.Get();
    USceneComponent* Parent = Saved.Parent.Get();
    if (!Rock || !Parent) return;
    Rock->SetSimulatePhysics(false);
    Rock->SetEnableGravity(false);
    ResetCollision(Rock, Saved.CollisionChannel);
    Rock->AttachToComponent(Parent, FAttachmentTransformRules::KeepRelativeTransform, Saved.Socket);
    Rock->SetRelativeTransform(Saved.Relative);
}

void FGolemCollapse::Begin(const TArray<UStaticMeshComponent*>& Rocks, float LinearImpulse, float SpinDegrees)
{
    if (bActive) return;
    bActive = true;
    RocksSaved.Reset();
    for (UStaticMeshComponent* Rock : Rocks)
    {
        if (IsValid(Rock))
        {
            RocksSaved.Add(Save(Rock));
            Rock->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            SetupCollision(Rock);
            Rock->SetSimulatePhysics(true);
            Rock->SetEnableGravity(true);
            const FVector Direction(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.25f);
            Rock->AddImpulse(Direction.GetSafeNormal() * LinearImpulse, NAME_None, true);
            Rock->AddAngularImpulseInDegrees(FMath::VRand() * SpinDegrees, NAME_None, true);
        }
    }
}

void FGolemCollapse::End()
{
    if (!bActive) return;
    bActive = false;
    for (const FSavedAttachment& Saved : RocksSaved) Restore(Saved);
    RocksSaved.Reset();
}
