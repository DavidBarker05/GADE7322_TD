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
            UMaterialInterface* MaterialInterface = MeshComponent->GetMaterial(i);
            if (!IsValid(MaterialInterface)) continue;
            UMaterialInstanceDynamic* MaterialInstanceDynamic =
                UMaterialInstanceDynamic::Create(MaterialInterface, this);
            MeshComponent->SetMaterial(i, MaterialInstanceDynamic);
            FlashMaterials.Add(MaterialInstanceDynamic);
            MaterialInstances.Add(MaterialInstanceDynamic);
        }
        OnMeshMaterialsAdded.Broadcast(MeshComponent, MaterialInstances);
    }
}

void UHitFlashComponent::UnbindMaterials()
{
    ForceStopFlash();
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

void UHitFlashComponent::ForceStopFlash()
{
    if (const UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(FlashHandle);
    EndFlash();
    bPaused = false;
}

bool UHitFlashComponent::IsStillFlashing() const
{
    if (const UWorld* World = GetWorld()) return World->GetTimerManager().IsTimerActive(FlashHandle);
    return false;
}

bool UHitFlashComponent::PauseFlash()
{
    if (bPaused) return true;
    if (!IsStillFlashing()) return false;
    if (const UWorld* World = GetWorld())
    {
        RemainingFlashDuration = World->GetTimerManager().GetTimerRemaining(FlashHandle);
        bPaused =
            !FMath::IsNearlyZero(RemainingFlashDuration); // If too small it might cause issues with the timer manager
        if (bPaused) World->GetTimerManager().ClearTimer(FlashHandle);
        else ForceStopFlash();
    }
    else ForceStopFlash();
    return bPaused;
}

void UHitFlashComponent::SetRemainingFlashDuration(float InRemainingFlashDuration)
{
    if (InRemainingFlashDuration <= SMALL_NUMBER) return;
    if (const UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(FlashHandle);
    bPaused = true;
    RemainingFlashDuration = InRemainingFlashDuration;
}

void UHitFlashComponent::ResumeFlash()
{
    if (!bPaused) return;
    bPaused = false;
    for (UMaterialInstanceDynamic* MID : FlashMaterials)
    {
        if (!IsValid(MID)) continue;
        MID->SetVectorParameterValue(TEXT("EmissiveFlashColor"), FlashColor);
        MID->SetScalarParameterValue(TEXT("EmissiveFlashMultiplier"), FlashIntensity);
    }
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(FlashHandle, this, &UHitFlashComponent::EndFlash, FlashDuration, false);
    }
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
