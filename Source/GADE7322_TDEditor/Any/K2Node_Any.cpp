#include "Any/K2Node_Any.h"
#include "Any/AnyFunctionLibrary.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_Any)

#define LOCTEXT_NAMESPACE "Any"

void UK2Node_Any::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    Super::GetMenuActions(ActionRegistrar);
    if (const UClass* Action = GetClass(); ActionRegistrar.IsOpenForRegistration(Action))
    {
        // ReSharper disable once CppParameterNeverUsed
        auto CustomizeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, const FName FunctionName) -> void
        {
            UK2Node_Any* Node = CastChecked<UK2Node_Any>(NewNode);
            const UFunction* Function = UAnyFunctionLibrary::StaticClass()->FindFunctionByName(FunctionName);
            check(Function);
            Node->SetFromFunction(Function);
        };
        // MakeAny()
        UBlueprintNodeSpawner* MakeNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(MakeNodeSpawner != nullptr);
        MakeNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(
            CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UAnyFunctionLibrary, MakeAny));
        ActionRegistrar.AddBlueprintAction(Action, MakeNodeSpawner);
        // AnyGetAsStruct()
        UBlueprintNodeSpawner* GetNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(GetNodeSpawner != nullptr);
        GetNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(
            CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UAnyFunctionLibrary, AnyGet));
        ActionRegistrar.AddBlueprintAction(Action, GetNodeSpawner);
        // SetAny()
        UBlueprintNodeSpawner* SetNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(SetNodeSpawner != nullptr);
        SetNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(
            CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UAnyFunctionLibrary, SetAny));
        ActionRegistrar.AddBlueprintAction(Action, SetNodeSpawner);
        // StructToAny()
        UBlueprintNodeSpawner* ToNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(ToNodeSpawner != nullptr);
        MakeNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(
            CustomizeLambda, GET_FUNCTION_NAME_CHECKED(UAnyFunctionLibrary, ToAny));
        ActionRegistrar.AddBlueprintAction(Action, ToNodeSpawner);
    }
}
#undef LOCTEXT_NAMESPACE
