// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameViewportClient.h"
#include "Engine/GameViewportClient.h"
#include "Misc/ScopeLock.h"



void UMyGameViewportClient::SetProperties(const FString& InputFilePath)
{
    UEFilePath = InputFilePath;
    FString hum = InputFilePath.Replace(TEXT("/"), TEXT("\\\\"));
    hum = hum.LeftChop(4);
    //check filepath
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, hum);
    std::string ConvertedFilePath(TCHAR_TO_UTF8(*hum));
    FilePath = ConvertedFilePath + ".avi";
    ViewportSize = Viewport->GetSizeXY();
}

void UMyGameViewportClient::Draw(FViewport* InViewport, FCanvas* SceneCanvas)
{

    Super::Draw(InViewport, SceneCanvas);

    if (Viewport == nullptr || SceneCanvas == nullptr)
    {
        // Add a debug message or print statement to verify function execution
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "ViewPort is Null");
        return;
    }

    if (doRecording)
    {
        // CaptureFrame();

         // Execute CaptureFrame on the game thread
            CaptureFrame();
    }

}

void UMyGameViewportClient::CaptureFrame()
{
    if (!Viewport)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ("No Viewport"));

        return;
    }

    if (ViewportSize.X == 0 || ViewportSize.Y == 0) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Viewport Size is 0");
        return;
    }

    //Clear Color Value
    ColorRec.Empty();

    if (!Viewport->ReadPixels(ColorRec, FReadSurfaceDataFlags(RCM_MinMax),
        FIntRect(0, 0, ViewportSize.X, ViewportSize.Y)))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Cannot Read From Viewport");
        return;
    }
    else
    {
        // Lock the mutex before enqueueing the data to ColorQueue
        FScopeLock Lock(&ColorBufferMutex);
        ColorQueue.Enqueue(ColorRec);
        // Release the lock when FScopeLock goes out of scope

    }
}

TQueue<TArray<FColor>>& UMyGameViewportClient::GetColorQueue()
{
    return ColorQueue;
}

void UMyGameViewportClient::VideoProcessing()
{
        if (ViewportSize.X != 0 && ViewportSize.Y != 0 && !FilePath.empty())
        {
            Rendering = true;
            recordingThread = new FMultiThreadRecording(ViewportSize, FilePath, this);
            CurrentRunningThread = FRunnableThread::Create(recordingThread, TEXT("Recording Thread"));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Fail To Create Thread");
        }
}


bool UMyGameViewportClient::FinishedRendering()
{
    Rendering = false;
    return Rendering;
}

void UMyGameViewportClient::ClearThread()
{
    ColorQueue.Empty();
    if (CurrentRunningThread && recordingThread)
    {
        CurrentRunningThread->WaitForCompletion();
        delete recordingThread;
        recordingThread = nullptr;
        CurrentRunningThread = nullptr;
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Thread Delete");

    }
}

