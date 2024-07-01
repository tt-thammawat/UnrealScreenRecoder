// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRecordingManager.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Engine/GameViewportClient.h"
#include "HAL/FileManager.h"

//casting
void UMyRecordingManager::CastToViewPort()
{
    GameViewportClient = GEngine->GameViewport;

    if (GameViewportClient)
    {
        MyViewportClient = Cast<UMyGameViewportClient>(GameViewportClient);
    }
}

//select folder
bool UMyRecordingManager::SelectSaveFolder(FString& OutFolder)
{
    // Get the desktop platform module
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    // Check if the desktop platform module is valid
    if (DesktopPlatform)
    {
        // Declare an array to store the selected folder path
        FString OutFolderPath;

        // Open the save file dialog with the given parameters and store the result
        bool bSuccess = DesktopPlatform->OpenDirectoryDialog(
            nullptr,
            TEXT("Select a folder to save the file in"),
            TEXT(""), // Default path (can be empty)
            OutFolderPath
        );

        // Check if the dialog was successful and a folder was selected
        if (bSuccess)
        {
            // Set the output folder path
            OutFolder = OutFolderPath;
        }

        // Return the result
        return bSuccess;
    }

    // Return false to indicate failure
    return false;
}

//select dir
bool UMyRecordingManager::SelectSaveDir(FString& OutFileName)
{
    // Get the desktop platform module
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    // Check if the desktop platform module is valid
    if (DesktopPlatform)
    {
        // Declare an array to store the selected file names
        TArray<FString> OutFileNames;

        // Append the wildcard option to the file types
        FString Filter = TEXT("AVI Files|*.avi");

        // Open the save file dialog with the given parameters and store the result
        bool bSuccess = DesktopPlatform->SaveFileDialog(
            nullptr,
            TEXT(""),
            TEXT(""),
            TEXT(""),
            Filter,
            EFileDialogFlags::None,
            OutFileNames
        );

        // Check if the dialog was successful and at least one file name was selected
        if (bSuccess && OutFileNames.Num() > 0)
        {
            // Set the output file name to the first selected file name
            OutFileName = OutFileNames[0];
        }

        // Return the result
        return bSuccess;

    }

    // Return false to indicate failure
    return false;
}

//set timestamp name
bool UMyRecordingManager::GetTimestamp(FString& OutTimestamp)
{
    // Get the current date and time as a string
    FString Timestamp = FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H.%M.%S"));

    // Check if the timestamp is not empty
    if (!Timestamp.IsEmpty())
    {
        OutTimestamp = Timestamp;
        return true; // Success
    }

    // Return false if there was an error getting the timestamp
    return false;
}

//set save file and send to multithread
void UMyRecordingManager::SetSaveFile(const FString& saveDir)
{
    if (MyViewportClient)
    {
        MyViewportClient->SetProperties(saveDir);
    }
}

//start recording
void UMyRecordingManager::DoRecording(bool bNewValue)
{
    if (MyViewportClient)
    {
        MyViewportClient->doRecording = bNewValue;
    }
}

//processing video
void UMyRecordingManager::VideoProcessing()
{
    if (MyViewportClient)
    {
        MyViewportClient->VideoProcessing();

        // Start the timer for the rendering check (repeat every 1 second)
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_RenderingCheck, this, &UMyRecordingManager::CheckRenderingStatus, 1.0f, true);
    }
}

//check if render
bool UMyRecordingManager::StillRendering()
{
        return MyViewportClient->Rendering;
}


//timer Check
void UMyRecordingManager::CheckRenderingStatus()
{
    if (MyViewportClient && !MyViewportClient->Rendering)
    {
        // Stop the timer
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RenderingCheck);

        // Call the ProcessingIsFinished function to trigger the delegate event
        FinishProcessing();
    }
}


//Delegate
void UMyRecordingManager::FinishProcessing()
{
    // Trigger the delegate event
    ProcessingIsFinished.Broadcast();
}


