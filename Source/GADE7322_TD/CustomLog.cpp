#include "CustomLog.h"

DEFINE_LOG_CATEGORY(LogCustom);

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
            UE_LOG(LogCustom, VeryVerbose, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Verbose:
#if WITH_EDITOR
            Colour = FColor::Cyan;
#endif
            UE_LOG(LogCustom, Verbose, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Log:
#if WITH_EDITOR
            Colour = FColor::Green;
#endif
            UE_LOG(LogCustom, Log, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Display:
#if WITH_EDITOR
            Colour = FColor::Green;
#endif
            UE_LOG(LogCustom, Display, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Warning:
#if WITH_EDITOR
            Colour = FColor::Yellow;
#endif
            UE_LOG(LogCustom, Warning, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Error:
#if WITH_EDITOR
            Colour = FColor::Red;
#endif
            UE_LOG(LogCustom, Error, TEXT("%s"), *Message);
            break;
        case ELogVerbosity::Fatal:
#if WITH_EDITOR
            Colour = FColor::Red;
#endif
            UE_LOG(LogCustom, Fatal, TEXT("%s"), *Message);
            break;
        default:
            return;
    }
#if WITH_EDITOR
    ELogVerbosity::Type CurrentVerbosity = LogCustom.GetVerbosity();
    if (CurrentVerbosity < Verbosity) return;
    GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, Colour, Message);
#endif
}
