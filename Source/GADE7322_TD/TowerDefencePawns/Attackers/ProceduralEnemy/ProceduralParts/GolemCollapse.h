#pragma once

#include "TDCore.h"

class UStaticMeshComponent;

struct FGolemCollapse final
{
    void Begin(const TArray<UStaticMeshComponent*>& Rocks, float LinearImpulse, float SpinDegrees);

    void End();

    bool IsActive() const { return bActive; }

private:
    struct FSavedAttachment
    {
        TWeakObjectPtr<UStaticMeshComponent> Rock;
        TWeakObjectPtr<USceneComponent> Parent;
        FTransform Relative;
        FName Socket;
        ECollisionChannel CollisionChannel;
    };

    static FSavedAttachment Save(UStaticMeshComponent* Rock);

    static void SetupCollision(UStaticMeshComponent* Rock);

    static void ResetCollision(UStaticMeshComponent* Rock, ECollisionChannel CollisionChannel);

    static void Restore(const FSavedAttachment& Saved);

    bool bActive = false;
    TArray<FSavedAttachment> RocksSaved;
};
