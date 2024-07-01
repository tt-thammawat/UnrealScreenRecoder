// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class myRecording : ModuleRules
{
    public myRecording(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        var thirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "Source", "ThirdParty"));

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...

			Path.Combine(thirdPartyPath, "myRecordingLibrary", "include")

            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "ImageWrapper",
                "myRecordingLibrary",
                "Projects"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				// ... add private dependencies that you statically link with here ...	
				"Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "Projects",
                "ImageWrapper",
                "Slate",
                "SlateCore",
                "myRecordingLibrary",
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

        string[] libs =
        {
        "avutil.lib",
        "avdevice.lib",
        "swresample.lib",
        "avcodec.lib",
        "swscale.lib",
        "avfilter.lib",
        "avformat.lib",
        "libcrypto.lib",
        "libssl.lib",
        "libx264.lib",
        "srt.lib"
};

        foreach (string lib in libs)
        {
            PublicSystemLibraries.Add(Path.Combine(thirdPartyPath, "myRecordingLibrary", "lib", lib));
        }

        string[] dlls = {
                "libcrypto-3-x64.dll",
                "libssl-3-x64.dll",
                "avutil-56.dll",
                "avdevice-58.dll",
                "swresample-3.dll",
                "avcodec-58.dll",
                "swscale-5.dll",
                "avfilter-7.dll",
                "avformat-58.dll",

                "libcrypto-1_1-x64.dll",
                "libssl-1_1-x64.dll",
                "srt.dll",
                "zlib1.dll",
                "libx264.dll",
            };

        foreach (string dll in dlls)
        {
            PublicDelayLoadDLLs.Add(dll);
            RuntimeDependencies.Add(Path.Combine(thirdPartyPath, "myRecordingLibrary", "bin", dll));
        }

    }
}
