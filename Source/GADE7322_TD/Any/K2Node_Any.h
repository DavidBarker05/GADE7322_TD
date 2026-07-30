#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Any.generated.h"

UCLASS()
class GADE7322_TD_API UK2Node_Any : public UK2Node_CallFunction
{
    GENERATED_BODY()

public:
    //~ Begin UEdGraphNode Interface
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    //~ End UEdGraphNode Interface

protected:
    //~ Begin UK2Node_CallFunction interface
    virtual bool CanToggleNodePurity() const override { return false; }
    //~ End UK2Node_CallFunction interface
};
