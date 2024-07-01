#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "MultiThreadRecording.h"
#include "HAL/RunnableThread.h"
#include "Hal/Runnable.h"
#include "Containers/Queue.h"
#include "HAL/CriticalSection.h"
#include "MyGameViewportClient.generated.h"

UCLASS(Config = Game, BlueprintType)
class MYRECORDING_API UMyGameViewportClient : public UGameViewportClient
{
    GENERATED_BODY()

public:
    TArray<FColor> ColorRec;
    TQueue<TArray<FColor>> ColorQueue;
    FCriticalSection ColorBufferMutex;

public:
    bool doRecording = false;
    bool Rendering = false;
    FIntPoint ViewportSize;
    std::string FilePath;
    FString UEFilePath;

public:
    virtual void Draw(FViewport* myViewport, FCanvas* SceneCanvas) override;

    void SetProperties(const FString& InputFilePath);

    void CaptureFrame();

    TQueue<TArray<FColor>>& GetColorQueue();

    void VideoProcessing();

    bool FinishedRendering();

    void ClearThread();
    //Thread
    class FMultiThreadRecording* recordingThread = nullptr;
    FRunnableThread* CurrentRunningThread = nullptr;

};
