#include "Any/AnyFunctionLibrary.h"

#include "Blueprint/BlueprintExceptionInfo.h"

#define LOCTEXT_NAMESPACE "UAnyFunctionLibrary"

FAny UAnyFunctionLibrary::MakeAny(const int32&)
{
    checkNoEntry();
    return {};
}

bool UAnyFunctionLibrary::Any_IsValid(const FAny& Any) { return Any.IsValid(); }

void UAnyFunctionLibrary::Any_IsValidBranch(const FAny& Any, EIsValidOutputPins& OutputPins)
{
    OutputPins = Any.IsValid() ? EIsValidOutputPins::Valid : EIsValidOutputPins::NotValid;
}

void UAnyFunctionLibrary::AnyGet(const FAny&, EIsAOutputPins&, int32&) { checkNoEntry(); }

FAny& UAnyFunctionLibrary::SetAny(FAny& InAny, const int32&)
{
    checkNoEntry();
    return InAny;
}

FAny UAnyFunctionLibrary::ToAny(const int32&)
{
    checkNoEntry();
    return {};
}

DEFINE_FUNCTION(UAnyFunctionLibrary::execMakeAny)
{
    Stack.MostRecentPropertyAddress = nullptr;
    Stack.MostRecentPropertyContainer = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    const FProperty* ValueProp = CastField<FProperty>(Stack.MostRecentProperty);
    const void* ValuePtr = Stack.MostRecentPropertyAddress;
    P_FINISH;
    bool bSuccess = false;
    if (ValueProp && ValuePtr)
    {
        P_NATIVE_BEGIN;
        FInstancedStruct Instanced;
        if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(ValueProp))
        {
            Instanced.InitializeAs<FBoolStruct>();
            if (FBoolStruct* BoolStructPtr = Instanced.GetMutablePtr<FBoolStruct>())
            {
                BoolStructPtr->Value = BoolProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FByteProperty* ByteProp = CastField<FByteProperty>(ValueProp))
        {
            Instanced.InitializeAs<FUint8Struct>();
            if (FUint8Struct* Uint8StructPtr = Instanced.GetMutablePtr<FUint8Struct>())
            {
                Uint8StructPtr->Value = ByteProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FIntProperty* IntProp = CastField<FIntProperty>(ValueProp))
        {
            Instanced.InitializeAs<FInt32Struct>();
            if (FInt32Struct* Int32StructPtr = Instanced.GetMutablePtr<FInt32Struct>())
            {
                Int32StructPtr->Value = IntProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FInt64Property* Int64Prop = CastField<FInt64Property>(ValueProp))
        {
            Instanced.InitializeAs<FInt64Struct>();
            if (FInt64Struct* Int64StructPtr = Instanced.GetMutablePtr<FInt64Struct>())
            {
                Int64StructPtr->Value = Int64Prop->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFloatStruct>();
            if (FFloatStruct* FloatStructPtr = Instanced.GetMutablePtr<FFloatStruct>())
            {
                FloatStructPtr->Value = FloatProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FStrProperty* StrProp = CastField<FStrProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFStringStruct>();
            if (FFStringStruct* StringStructPtr = Instanced.GetMutablePtr<FFStringStruct>())
            {
                StringStructPtr->Value = StrProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FNameProperty* NameProp = CastField<FNameProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFNameStruct>();
            if (FFNameStruct* NameStructPtr = Instanced.GetMutablePtr<FFNameStruct>())
            {
                NameStructPtr->Value = NameProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (CastField<FTextProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFTextStruct>();
            if (FFTextStruct* TextStructPtr = Instanced.GetMutablePtr<FFTextStruct>())
            {
                TextStructPtr->Value = *static_cast<const FText*>(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(ValueProp))
        {
            Instanced.InitializeAs<FUObjectStruct>();
            if (FUObjectStruct* ObjectStructPtr = Instanced.GetMutablePtr<FUObjectStruct>())
            {
                ObjectStructPtr->Value = ObjectProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
        {
            Instanced.InitializeAs(StructProp->Struct);
            StructProp->Struct->CopyScriptStruct(Instanced.GetMutableMemory(), ValuePtr);
            bSuccess = true;
        }
        if (bSuccess) *static_cast<FAny*>(RESULT_PARAM) = FAny(MoveTemp(Instanced));
        P_NATIVE_END;
    }
    if (!bSuccess)
    {
        const FBlueprintExceptionInfo ExceptionInfo(
            EBlueprintExceptionType::AbortExecution,
            LOCTEXT("AnyMake_MakeInvalidValueWarning", "Failed to resolve the Value for Make Any"));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
        P_NATIVE_BEGIN;
        *static_cast<FAny*>(RESULT_PARAM) = FAny();
        P_NATIVE_END;
    }
}

DEFINE_FUNCTION(UAnyFunctionLibrary::execAnyGet)
{
    P_GET_STRUCT_REF(FAny, InAny);
    P_GET_ENUM_REF(EIsAOutputPins, OutputPins);
    Stack.MostRecentPropertyAddress = nullptr;
    Stack.MostRecentPropertyContainer = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    const FProperty* ValueProp = CastField<FProperty>(Stack.MostRecentProperty);
    void* ValuePtr = Stack.MostRecentPropertyAddress;
    P_FINISH;
    OutputPins = EIsAOutputPins::IsNotType;
    if (ValueProp && ValuePtr && InAny.Value.IsValid())
    {
        bool bSuccess = false;
        P_NATIVE_BEGIN;
        if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(ValueProp))
        {
            if (const bool* InnerValue = InAny.Get<bool>())
            {
                BoolProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (const FByteProperty* ByteProp = CastField<FByteProperty>(ValueProp))
        {
            if (const uint8* InnerValue = InAny.Get<uint8>())
            {
                ByteProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (const FIntProperty* IntProp = CastField<FIntProperty>(ValueProp))
        {
            if (const int32* InnerValue = InAny.Get<int32>())
            {
                IntProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (CastField<FInt64Property>(ValueProp))
        {
            if (const int64* InnerValue = InAny.Get<int64>())
            {
                IntProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(ValueProp))
        {
            if (const float* InnerValue = InAny.Get<float>())
            {
                FloatProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (const FStrProperty* StrProp = CastField<FStrProperty>(ValueProp))
        {
            if (const FString* InnerValue = InAny.Get<FString>())
            {
                StrProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (CastField<FTextProperty>(ValueProp))
        {
            if (const FText* InnerValue = InAny.Get<FText>())
            {
                *static_cast<FText*>(ValuePtr) = *InnerValue;
                bSuccess = true;
            }
        }
        else if (const FNameProperty* NameProp = CastField<FNameProperty>(ValueProp))
        {
            if (const FName* InnerValue = InAny.Get<FName>())
            {
                NameProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(ValueProp))
        {
            if (const TObjectPtr<UObject>* InnerValue = InAny.Get<TObjectPtr<UObject>>())
            {
                ObjectProp->SetPropertyValue(ValuePtr, *InnerValue);
                bSuccess = true;
            }
        }
        else if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
        {
            if (InAny.Value.GetScriptStruct()->IsChildOf(StructProp->Struct))
            {
                StructProp->Struct->CopyScriptStruct(ValuePtr, InAny.Value.GetMemory());
            }
        }
        OutputPins = bSuccess ? EIsAOutputPins::IsType : EIsAOutputPins::IsNotType;
        P_NATIVE_END;
    }
}

DEFINE_FUNCTION(UAnyFunctionLibrary::execSetAny)
{
    P_GET_STRUCT_REF(FAny, InAny);
    Stack.MostRecentPropertyAddress = nullptr;
    Stack.MostRecentPropertyContainer = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    const FProperty* ValueProp = CastField<FProperty>(Stack.MostRecentProperty);
    const void* ValuePtr = Stack.MostRecentPropertyAddress;
    P_FINISH;
    bool bSuccess = false;
    if (ValueProp && ValuePtr)
    {
        P_NATIVE_BEGIN;
        if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(ValueProp))
        {
            InAny = BoolProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FByteProperty* ByteProp = CastField<FByteProperty>(ValueProp))
        {
            InAny = ByteProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FIntProperty* IntProp = CastField<FIntProperty>(ValueProp))
        {
            InAny = IntProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FInt64Property* Int64Prop = CastField<FInt64Property>(ValueProp))
        {
            InAny = Int64Prop->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(ValueProp))
        {
            InAny = FloatProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FStrProperty* StrProp = CastField<FStrProperty>(ValueProp))
        {
            InAny = StrProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FNameProperty* NameProp = CastField<FNameProperty>(ValueProp))
        {
            InAny = NameProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (CastField<FTextProperty>(ValueProp))
        {
            InAny.Set(*static_cast<const FText*>(ValuePtr));
            bSuccess = true;
        }
        else if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(ValueProp))
        {
            InAny = ObjectProp->GetPropertyValue(ValuePtr);
            bSuccess = true;
        }
        else if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
        {
            InAny.Value.InitializeAs(StructProp->Struct);
            StructProp->Struct->CopyScriptStruct(InAny.Value.GetMutableMemory(), ValuePtr);
            bSuccess = true;
        }
        if (bSuccess) *static_cast<FAny*>(RESULT_PARAM) = InAny;
        P_NATIVE_END;
    }
    if (!bSuccess)
    {
        const FBlueprintExceptionInfo ExceptionInfo(
            EBlueprintExceptionType::AbortExecution,
            LOCTEXT("InstancedStruct_SetInvalidValueWarning", "Failed to resolve the Value for Set Any"));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
        P_NATIVE_BEGIN;
        *static_cast<FAny*>(RESULT_PARAM) = InAny;
        P_NATIVE_END;
    }
}

DEFINE_FUNCTION(UAnyFunctionLibrary::execToAny)
{
    Stack.MostRecentPropertyAddress = nullptr;
    Stack.MostRecentPropertyContainer = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    const FProperty* ValueProp = CastField<FProperty>(Stack.MostRecentProperty);
    const void* ValuePtr = Stack.MostRecentPropertyAddress;
    P_FINISH;
    bool bSuccess = false;
    if (ValueProp && ValuePtr)
    {
        P_NATIVE_BEGIN;
        FInstancedStruct Instanced;
        if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(ValueProp))
        {
            Instanced.InitializeAs<FBoolStruct>();
            if (FBoolStruct* BoolStructPtr = Instanced.GetMutablePtr<FBoolStruct>())
            {
                BoolStructPtr->Value = BoolProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FByteProperty* ByteProp = CastField<FByteProperty>(ValueProp))
        {
            Instanced.InitializeAs<FUint8Struct>();
            if (FUint8Struct* Uint8StructPtr = Instanced.GetMutablePtr<FUint8Struct>())
            {
                Uint8StructPtr->Value = ByteProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FIntProperty* IntProp = CastField<FIntProperty>(ValueProp))
        {
            Instanced.InitializeAs<FInt32Struct>();
            if (FInt32Struct* Int32StructPtr = Instanced.GetMutablePtr<FInt32Struct>())
            {
                Int32StructPtr->Value = IntProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FInt64Property* Int64Prop = CastField<FInt64Property>(ValueProp))
        {
            Instanced.InitializeAs<FInt64Struct>();
            if (FInt64Struct* Int64StructPtr = Instanced.GetMutablePtr<FInt64Struct>())
            {
                Int64StructPtr->Value = Int64Prop->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFloatStruct>();
            if (FFloatStruct* FloatStructPtr = Instanced.GetMutablePtr<FFloatStruct>())
            {
                FloatStructPtr->Value = FloatProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FStrProperty* StrProp = CastField<FStrProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFStringStruct>();
            if (FFStringStruct* StringStructPtr = Instanced.GetMutablePtr<FFStringStruct>())
            {
                StringStructPtr->Value = StrProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FNameProperty* NameProp = CastField<FNameProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFNameStruct>();
            if (FFNameStruct* NameStructPtr = Instanced.GetMutablePtr<FFNameStruct>())
            {
                NameStructPtr->Value = NameProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (CastField<FTextProperty>(ValueProp))
        {
            Instanced.InitializeAs<FFTextStruct>();
            if (FFTextStruct* TextStructPtr = Instanced.GetMutablePtr<FFTextStruct>())
            {
                TextStructPtr->Value = *static_cast<const FText*>(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(ValueProp))
        {
            Instanced.InitializeAs<FUObjectStruct>();
            if (FUObjectStruct* ObjectStructPtr = Instanced.GetMutablePtr<FUObjectStruct>())
            {
                ObjectStructPtr->Value = ObjectProp->GetPropertyValue(ValuePtr);
                bSuccess = true;
            }
        }
        else if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
        {
            Instanced.InitializeAs(StructProp->Struct);
            StructProp->Struct->CopyScriptStruct(Instanced.GetMutableMemory(), ValuePtr);
            bSuccess = true;
        }
        if (bSuccess) *static_cast<FAny*>(RESULT_PARAM) = FAny(MoveTemp(Instanced));
        P_NATIVE_END;
    }
    if (!bSuccess)
    {
        const FBlueprintExceptionInfo ExceptionInfo(
            EBlueprintExceptionType::AbortExecution,
            LOCTEXT("InstancedStruct_SetInvalidValueWarning", "Failed to resolve the Value for To Any"));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
        P_NATIVE_BEGIN;
        *static_cast<FAny*>(RESULT_PARAM) = FAny();
        P_NATIVE_END;
    }
}

#undef LOCTEXT_NAMESPACE
