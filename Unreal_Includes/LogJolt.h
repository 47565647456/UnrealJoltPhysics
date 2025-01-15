#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include "Misc/DateTime.h"

JOLTUNREAL_API DECLARE_LOG_CATEGORY_EXTERN(LogJolt, Display, All);

// Helper macro to get clean function name
#define JOLT_FUNC_NAME (FString(__FUNCSIG__).Mid(FString(__FUNCSIG__).Find(TEXT(":")), FString(__FUNCSIG__).Len()))

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

// Main logging macro with additional context
#define JOLT_LOG(LogCategory, Verbosity, Format, ...) \
{ \
    const FString TimeStamp = FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H.%M.%S.%s")); \
    const FString ThreadId = *FString::Printf(TEXT("0x%08x"), FPlatformTLS::GetCurrentThreadId()); \
    \
    UE_LOGFMT(LogCategory, Verbosity, "{TimeStamp} [{ThreadId}] {Function}: " Format, \
    *TimeStamp, *ThreadId, *JOLT_FUNC_NAME, ##__VA_ARGS__); \
}

// Specialized logging macros for different verbosity levels
#define JOLT_VERBOSE(LogCategory, Format, ...)   JOLT_LOG(LogCategory, Verbose, Format, ##__VA_ARGS__)
#define JOLT_DEBUG(LogCategory, Format, ...)     JOLT_LOG(LogCategory, Log, Format, ##__VA_ARGS__)
#define JOLT_INFO(LogCategory, Format, ...)      JOLT_LOG(LogCategory, Display, Format, ##__VA_ARGS__)
#define JOLT_WARNING(LogCategory, Format, ...)   JOLT_LOG(LogCategory, Warning, Format, ##__VA_ARGS__)
#define JOLT_ERROR(LogCategory, Format, ...)     JOLT_LOG(LogCategory, Error, Format, ##__VA_ARGS__)

// Assertion macro with logging
#define JOLT_ENSURE(Condition, LogCategory, Format, ...) \
{ \
    if (!(Condition)) \
    { \
        JOLT_ERROR(LogCategory, "Assertion failed: " #Condition ". " Format, ##__VA_ARGS__); \
        ensure(false); \
    } \
}

// Check macro with logging
#define JOLT_CHECK(Condition, LogCategory, Format, ...) \
{ \
    if (!(Condition)) \
    { \
        JOLT_ERROR(LogCategory, "Assertion failed: " #Condition ". " Format, ##__VA_ARGS__); \
        check(false); \
    } \
}

#else
#define JOLT_LOG(LogCategory, Verbosity, Format, ...)

#define JOLT_VERBOSE(LogCategory, Format, ...)
#define JOLT_DEBUG(LogCategory, Format, ...)   
#define JOLT_INFO(LogCategory, Format, ...)    
#define JOLT_WARNING(LogCategory, Format, ...) 
#define JOLT_ERROR(LogCategory, Format, ...)

#define JOLT_ENSURE(Condition, LogCategory, Format, ...)
#define JOLT_CHECK(Condition, LogCategory, Format, ...)
#endif
