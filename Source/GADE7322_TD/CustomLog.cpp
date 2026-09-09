#include "CustomLog.h"

DEFINE_LOG_CATEGORY(TowerDefenceLog);

// ReSharper disable once CppParameterMayBeConst
void FCustomLog::Log(ELogVerbosity::Type Verbosity, const FString& Message)
{
#if WITH_EDITOR
    FColor Colour;
#endif
    switch (Verbosity)
    {
        case ELogVerbosity::VeryVerbose:
#if WITH_EDITOR
            Colour = FColor::White;
#endif
            UE_LOG(TowerDefenceLog, VeryVerbose, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Verbose:
#if WITH_EDITOR
            Colour = FColor::Cyan;
#endif
            UE_LOG(TowerDefenceLog, Verbose, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Log:
#if WITH_EDITOR
            Colour = FColor::Green;
#endif
            UE_LOG(TowerDefenceLog, Log, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Display:
#if WITH_EDITOR
            Colour = FColor::Green;
#endif
            UE_LOG(TowerDefenceLog, Display, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Warning:
#if WITH_EDITOR
            Colour = FColor::Yellow;
#endif
            UE_LOG(TowerDefenceLog, Warning, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Error:
#if WITH_EDITOR
            Colour = FColor::Red;
#endif
            UE_LOG(TowerDefenceLog, Error, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Fatal:
#if WITH_EDITOR
            Colour = FColor::Red;
#endif
            UE_LOG(TowerDefenceLog, Fatal, TEXT("%s"), *Message);
            break;
        default:
            return;
    }
#if WITH_EDITOR
    if (const ELogVerbosity::Type CurrentVerbosity = TowerDefenceLog.GetVerbosity(); CurrentVerbosity < Verbosity)
        return;
    GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, Colour, Message);
#endif
}
