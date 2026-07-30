#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include <format>

#ifndef PRINT_TO_SCREEN
// Just useful way to print onto screen using c++20 formatter
#define PRINT_TO_SCREEN(Key, Duration, Color, Message, ...) GEngine->AddOnScreenDebugMessage(Key, Duration, Color, FString(std::format(Message, __VA_ARGS__).c_str()))
#endif
