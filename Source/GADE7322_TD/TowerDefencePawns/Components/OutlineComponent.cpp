#include "TowerDefencePawns/Components/OutlineComponent.h"

void UOutlineComponent::BindMaterials()
{
    UnbindMaterials(); // Unbind in case any materials already
    AActor* Owner = GetOwner();
    if (!Owner) return;
    for (auto& MeshOutlinePair : MeshOutlinePairs)
    {
        FComponentReference Ref = MeshOutlinePair.GetReference();
        UMaterialInterface* MaterialInterface = MeshOutlinePair.GetOutlineMaterial();
        if (!MaterialInterface) continue;
        UMeshComponent* MeshComponent = Cast<UMeshComponent>(Ref.GetComponent(Owner));
        if (!IsValid(MeshComponent)) continue;
        UMaterialInstanceDynamic* OutlineMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, this);
        MeshComponent->SetOverlayMaterial(OutlineMaterial);
        OutlineMaterials.Add(OutlineMaterial);
    }
}

void UOutlineComponent::UnbindMaterials()
{
    DisableOutline();
    OutlineMaterials.Empty();
}

void UOutlineComponent::EnableOutline()
{
    for (const auto OutlineMaterial : OutlineMaterials)
    {
        if (IsValid(OutlineMaterial)) OutlineMaterial->SetScalarParameterValue("Enabled", 1.0f);
    }
}

void UOutlineComponent::DisableOutline()
{
    for (const auto OutlineMaterial : OutlineMaterials)
    {
        if (IsValid(OutlineMaterial)) OutlineMaterial->SetScalarParameterValue("Enabled", 0.0f);
    }
}
