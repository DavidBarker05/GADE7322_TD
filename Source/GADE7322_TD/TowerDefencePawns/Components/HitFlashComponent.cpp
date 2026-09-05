#include "TowerDefencePawns/Components/HitFlashComponent.h"

#include "Components/MeshComponent.h"

void UHitFlashComponent::BindMaterials()
{
    UnbindMaterials(); // Unbind in case any materials already
    AActor* Owner = GetOwner();
    if (!Owner) return;
    for (const FComponentReference& Ref : MeshComponents)
    {
        UMeshComponent* MC = Cast<UMeshComponent>(Ref.GetComponent(Owner));
        if (!IsValid(MC)) continue;
        MC->EmptyOverrideMaterials();
        for (int32 i = 0; i < MC->GetNumMaterials(); ++i)
        {
            UMaterialInterface* MI = MC->GetMaterial(i);
            if (!IsValid(MI)) continue;
            UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(MI, this);
            MC->SetMaterial(i, MID);
            FlashMaterials.Add(MID);
        }
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
