// Copyright Epic Games, Inc. All Rights Reserved.

#include "myRecording.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "FmyRecordingModule"

void FmyRecordingModule::StartupModule()
{
	FString LoadedDlls;

	FString PluginDirectory = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("myRecording"));
	TArray<FString> DllPaths = { "avutil-56.dll","libcrypto-1_1-x64.dll","libcrypto-3-x64.dll","libssl-1_1-x64.dll","libssl-3-x64.dll", "srt.dll","swresample-3.dll","swscale-5.dll","zlib1.dll","avcodec-58.dll","avformat-58.dll","avfilter-7.dll","avdevice-58.dll","libx264.dll" };

	for (const FString& DllPath : DllPaths)
	{
		FString FullDllPath = FPaths::Combine(PluginDirectory, TEXT("Source/ThirdParty/myRecordingLibrary/bin/"), DllPath);
		DllHandle = FPlatformProcess::GetDllHandle(*FullDllPath);
		//if (DllHandle != nullptr)
		//{
		//	// DLL loaded successfully
		//	LoadedDlls += DllPath + TEXT("\n");

		//}
		//else
		//{
		//	LoadedDlls += TEXT("Fail ")+DllPath + TEXT("\n");

		//}

	}
	//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(LoadedDlls));

}

void FmyRecordingModule::ShutdownModule()
{
	FPlatformProcess::FreeDllHandle(DllHandle);

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FmyRecordingModule, myRecording)
