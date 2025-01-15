using System.IO;
using System.Linq;
using UnrealBuildTool;


/*
 * 
 * WARNING! IF YOU CHANGE ANY OF THE CMAKE FLAGS, MAKE SURE TO DELETE THE CMAKE FOLDER IN YOUR INTERMEDIATE DIRECTORY.
 * I HAVEN'T FOUND A WAY TO TRIGGER REBUILDS WHEN CHANGING A FLAG!
 * 
 */

public class LibJolt : ModuleRules
{
    public string GetJoltArgs(ReadOnlyTargetRules Target)
    {
        string cmakeFlags = "";
        if (Target.Configuration == UnrealTargetConfiguration.Debug) // untested
        {
            cmakeFlags += " -DCMAKE_BUILD_TYPE=Debug ";
            cmakeFlags += " -DUSE_ASSERTS=ON ";
            cmakeFlags += " -DGENERATE_DEBUG_SYMBOLS=ON ";
            //cmakeFlags += " -DOVERRIDE_CXX_FLAGS=OFF "; // probably may be omitted
            //cmakeFlags += " -DCMAKE_CXX_FLAGS_RELEASE=\" /GS /Od /Ob0 /RTC1\" "; // probably may be omitted
            cmakeFlags += " -DDEBUG_RENDERER=ON ";
            cmakeFlags += " -DPROFILE_ENABLED=ON ";
            cmakeFlags += " -DENABLE_ASSERTS=ON ";
        }
        else if (Target.Configuration == UnrealTargetConfiguration.DebugGame || Target.Configuration == UnrealTargetConfiguration.Development)
        {
            cmakeFlags += " -DCMAKE_BUILD_TYPE=Release ";
            cmakeFlags += " -DUSE_ASSERTS=ON ";
            cmakeFlags += " -DGENERATE_DEBUG_SYMBOLS=ON ";
            //cmakeFlags += " -DOVERRIDE_CXX_FLAGS=OFF "; // probably may be omitted
            //cmakeFlags += " -DCMAKE_CXX_FLAGS_RELEASE=\" /GS /Od /Ob0 /RTC1\" "; // probably may be omitted
            cmakeFlags += " -DDEBUG_RENDERER=ON ";
            cmakeFlags += " -DPROFILE_ENABLED=ON ";
            cmakeFlags += " -DENABLE_ASSERTS=ON ";
        }
        else
        {
            // FOR NOW CAN'T USE DISTRIBUTION!
            //cmakeFlags += " -DCMAKE_BUILD_TYPE=Distribution ";
            cmakeFlags += " -DCMAKE_BUILD_TYPE=Release ";
            cmakeFlags += " -DDEBUG_RENDERER=OFF ";
            cmakeFlags += " -DPROFILE_ENABLED=OFF ";
            cmakeFlags += " -DENABLE_ASSERTS=ON ";
        }

        cmakeFlags += " -DFLOATING_POINT_EXCEPTIONS_ENABLED=OFF ";
        cmakeFlags += " -DCROSS_PLATFORM_DETERMINISTIC=ON ";
        cmakeFlags += " -DOBJECT_STREAM=ON ";
        cmakeFlags += " -DDOUBLE_PRECISION=OFF "; // Single precision

        // can be 16 or 32
        cmakeFlags += " -DOBJECT_LAYER_BITS=32 ";

        // LTO
        cmakeFlags += " -DINTERPROCEDURAL_OPTIMIZATION=ON ";


        cmakeFlags += " -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ";

        cmakeFlags += " -DTARGET_SAMPLES=OFF ";
        cmakeFlags += " -DTARGET_HELLO_WORLD=OFF ";
        cmakeFlags += " -DTARGET_VIEWER=OFF ";
        cmakeFlags += " -DTARGET_UNIT_TESTS=OFF ";
        cmakeFlags += " -DTARGET_PERFORMANCE_TEST=OFF ";

        // SIMD
        var simdStatus = "ON ";
        cmakeFlags += " -DUSE_AVX512=OFF ";
        cmakeFlags += " -DUSE_AVX2=OFF ";
        cmakeFlags += " -DUSE_AVX=OFF ";
        cmakeFlags += " -DUSE_FMADD=OFF ";
        cmakeFlags += " -DUSE_SSE4_2=" + simdStatus;
        cmakeFlags += " -DUSE_SSE4_1=" + simdStatus;
        cmakeFlags += " -DUSE_LZCNT=" + simdStatus;
        cmakeFlags += " -DUSE_TZCNT=" + simdStatus;
        cmakeFlags += " -DUSE_F16C=" + simdStatus;

        cmakeFlags += " -DENABLE_ALL_WARNINGS=ON ";
        cmakeFlags += " -DUSE_STATIC_MSVC_RUNTIME_LIBRARY=OFF ";
        cmakeFlags += " -DBUILD_SHARED_LIBS=ON ";

        return cmakeFlags;
    }

    public LibJolt(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        CppStandard = CppStandardVersion.Cpp17;

        CMakeTarget.add(Target, this, "Jolt", Path.Combine(ModuleDirectory, "Build"), GetJoltArgs(Target));

        // We don't need these defines in this module. These are defined by Jolt.
        PublicDefinitions.Remove("_HAS_EXCEPTIONS=0");
        PublicDefinitions.Remove("NDEBUG");
        
        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "Jolt"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Unreal_Includes"));
        
        

        string DependencyPath = PublicRuntimeLibraryPaths.Last();
        DirectoryInfo d = new DirectoryInfo(@DependencyPath.Replace("\"", ""));

        foreach (var dllFile in d.GetFiles("*.dll", SearchOption.AllDirectories))
        {
            RuntimeDependencies.Add($"$(TargetOutputDir)/{dllFile.Name}", dllFile.FullName);
        }
    }
}
