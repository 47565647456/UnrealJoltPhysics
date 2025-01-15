#pragma once


#include "Unreal_Includes/JoltCommon.h"

#include "Log/JoltLog.h"

static void JoltTrace(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	JOLT_ERROR(LogJolt, "{0}", FString(buffer));
}

static bool JoltAssert(const char* Expr, const char* Msg, const char* File, uint32_t LineNum)
{
	// @formatter:off
	JOLT_ERROR(LogJolt,
		"Jolt assertion '{0}' triggered. Message '{1}' at '{2}:{3}' ",
		FString(Expr),
		Msg != nullptr ? FString(Msg) : "",
		FString(File),
		LineNum);
	// @formatter:on

	return false;
}

JOLTUNREAL_API void* JoltMalloc(SIZE_T Count)
{
	return FMemory::Malloc(Count);
}

JOLTUNREAL_API void* JoltRealloc(void *inBlock, size_t inOldSize, size_t inNewSize)
{
	return FMemory::Realloc(inBlock, inNewSize);
}

JOLTUNREAL_API void* JoltAlignedAlloc(size_t inSize, size_t inAlignment)
{
	return FMemory::Malloc(inSize, inAlignment);
}

JOLTUNREAL_API void JoltFree(void *inBlock)
{
	FMemory::Free(inBlock);
}

static void InitializeJolt()
{	
	verify(JPH::VerifyJoltVersionID());

	// This needs to be done before any other Jolt function is called
	JPH::Allocate = &JoltMalloc;
	JPH::Reallocate = &JoltRealloc;
	JPH::Free = &JoltFree;
	JPH::AlignedAllocate = &JoltAlignedAlloc;
	JPH::AlignedFree = &JoltFree;

	// Install trace and assert callbacks
	JPH::Trace = &JoltTrace;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = &JoltAssert;)

	// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
	JPH::Factory::sInstance = new JPH::Factory;

	// // Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	// // If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
	// // If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
	 JPH::RegisterTypes();

}

static void CleanupJolt()
{
	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	// Destroy the factory
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	// I don't know if this necessary. I don't think it is.
	JPH::AlignedFree = nullptr;
	JPH::AlignedAllocate = nullptr;
	JPH::Free = nullptr;
	JPH::Reallocate = nullptr;
	JPH::Allocate = nullptr;
}