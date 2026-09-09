#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(TowerDefenceLog, Log, All);

#ifndef TD_LOG_NDSP
// Log a message to console/log file only
#define TD_LOG_NDSP(Verbosity, Format, ...) \
    do \
    { \
        UE_LOG(TowerDefenceLog, Verbosity, Format __VA_OPT__(, ) __VA_ARGS__) \
    } while (0)
#endif

#ifndef TD_LOG_INFO_NDSP
// Log a message at the Display verbosity to console/log file only
#define TD_LOG_INFO_NDSP(Format, ...) \
    do \
    { \
        UE_LOG(TowerDefenceLog, Display, Format __VA_OPT__(, ) __VA_ARGS__) \
    } while (0)
#endif

#ifndef TD_LOG_WARN_NDSP
// Log a message at the Warning verbosity to console/log file only
#define TD_LOG_WARN_NDSP(Format, ...) \
    do \
    { \
        UE_LOG(TowerDefenceLog, Warning, Format __VA_OPT__(, ) __VA_ARGS__) \
    } while (0)
#endif

#ifndef TD_LOG_ERROR_NDSP
// Log a message at the Error verbosity to console/log file only
#define TD_LOG_ERROR_NDSP(Format, ...) \
    do \
    { \
        UE_LOG(TowerDefenceLog, Error, Format __VA_OPT__(, ) __VA_ARGS__) \
    } while (0)
#endif

#ifndef TD_LOG_FATAL_NDSP
// Log a message at the Fatal verbosity to console/log file only
#define TD_LOG_FATAL_NDSP(Format, ...) \
    do \
    { \
        UE_LOG(TowerDefenceLog, Fatal, Format __VA_OPT__(, ) __VA_ARGS__) \
    } while (0)
#endif

#if WITH_EDITOR
#ifndef TD_LOG
// Log a message at to screen and console/log file
#define TD_LOG(Verbosity, Format, ...) \
    FCustomLog::Log(ELogVerbosity::Verbosity, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))
#endif

#ifndef TD_LOG_INFO
// Log a message at the Display verbosity to screen and console/log file
#define TD_LOG_INFO(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Display, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))
#endif

#ifndef TD_LOG_WARN
// Log a message at the Warning verbosity to screen and console/log file
#define TD_LOG_WARN(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Warning, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))
#endif

#ifndef TD_LOG_ERROR
// Log a message at the Error verbosity to screen and console/log file
#define TD_LOG_ERROR(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Error, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))
#endif

#ifndef TD_LOG_FATAL
// Log a message at the Fatal verbosity to screen and console/log file
#define TD_LOG_FATAL(Format, ...) \
    FCustomLog::Log(ELogVerbosity::Fatal, FString::Printf(Format __VA_OPT__(, ) __VA_ARGS__))
#endif
#else
#ifndef TD_LOG
// Log a message to console/log file only
#define TD_LOG(Verbosity, Format, ...) TD_LOG_NDSP(Verbosity, Format __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TD_LOG_INFO
// Log a message at the Display verbosity to console/log file only
#define TD_LOG_INFO(Format, ...) TD_LOG_INFO_NDSP(Format __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TD_LOG_WARN
// Log a message at the Warning verbosity to console/log file only
#define TD_LOG_WARN(Format, ...) TD_LOG_WARN_NDSP(Format __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TD_LOG_ERROR
// Log a message at the Error verbosity to console/log file only
#define TD_LOG_ERROR(Format, ...) TD_LOG_ERROR_NDSP(Format __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TD_LOG_FATAL
// Log a message at the Fatal verbosity to console/log file only
#define TD_LOG_FATAL(Format, ...) TD_LOG_FATAL_NDSP(Format __VA_OPT__(, ) __VA_ARGS__)
#endif
#endif

// Custom log that outputs to GEngine and UE_LOG
struct FCustomLog
{
    // Log a message
    static void Log(ELogVerbosity::Type Verbosity, const FString& Message);
};
