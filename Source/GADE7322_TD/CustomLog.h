#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);

#define TD_LOG(Verbosity, Format, ...) \
    FCustomLog::Log(ELogVerbosity::Verbosity, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))

// Log a message at the Display verbosity
#define TD_LOG_INFO(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Display, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))

// Log a message at the Warning verbosity
#define TD_LOG_WARN(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Warning, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))

// Log a message at the Error verbosity
#define TD_LOG_ERROR(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Error, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))

// Log a message at the Fatal verbosity
#define TD_LOG_FATAL(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Fatal, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))

// Custom log that outputs to GEngine and UE_LOG
struct FCustomLog
{
    // Log a message
    static void Log(ELogVerbosity::Type Verbosity, const FString& Message);
};
