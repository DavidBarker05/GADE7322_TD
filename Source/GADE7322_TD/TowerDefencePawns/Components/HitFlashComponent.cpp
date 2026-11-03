#include "TowerDefencePawns/Components/HitFlashComponent.h"

void UHitFlashComponent::BindMaterials()
{
    UnbindMaterials(); // Unbind in case any materials already
    AActor* Owner = GetOwner();
    if (!Owner) return;
    for (const FComponentReference& Ref : MeshComponents)
    {
        UMeshComponent* MeshComponent = Cast<UMeshComponent>(Ref.GetComponent(Owner));
        if (!IsValid(MeshComponent)) continue;
        MeshComponent->EmptyOverrideMaterials();
        TArray<UMaterialInstanceDynamic*> MaterialInstances;
        for (int32 i = 0; i < MeshComponent->GetNumMaterials(); ++i)
        {
            UMaterialInterface* MaterialInterfaceI = MeshComponent->GetMaterial(i);
            if (!IsValid(MaterialInterfaceI)) continue;
            UMaterialInstanceDynamic* MaterialInstanceDynamic =
                UMaterialInstanceDynamic::Create(MaterialInterfaceI, this);
            MeshComponent->SetMaterial(i, MaterialInstanceDynamic);
            FlashMaterials.Add(MaterialInstanceDynamic);
            MaterialInstances.Add(MaterialInstanceDynamic);
        }
        OnMeshMaterialsAdded.Broadcast(MeshComponent, MaterialInstances);
    }
}

void UHitFlashComponent::UnbindMaterials()
{
    EndFlash();
    if (const UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(FlashHandle);
    FlashMaterials.Empty();
}

void UHitFlashComponent::DoFlash()
{
    for (UMaterialInstanceDynamic* MID : FlashMaterials)
    {
        if (!IsValid(MID)) continue;
        MID->SetVectorParameterValue(TEXT("EmissiveFlashColor"), FlashColor);
        MID->SetScalarParameterValue(TEXT("EmissiveFlashMultiplier"), FlashIntensity);
    }
    if (const UWorld* World = GetWorld())
        World->GetTimerManager().SetTimer(FlashHandle, this, &UHitFlashComponent::EndFlash, FlashDuration, false);
}

void UHitFlashComponent::EndFlash()
{
    for (UMaterialInstanceDynamic* MID : FlashMaterials)
    {
        if (!IsValid(MID)) continue;
        MID->SetVectorParameterValue(TEXT("EmissiveFlashColor"), FLinearColor::Black);
        MID->SetScalarParameterValue(TEXT("EmissiveFlashMultiplier"), 0.0f);
    }
}
