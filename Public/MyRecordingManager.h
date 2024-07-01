// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyGameViewportClient.h"
#include "Delegates/Delegate.h"
#include <string>
#include "MyRecordingManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyDelegate);

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYRECORDING_API UMyRecordingManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		void CastToViewPort();
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		bool SelectSaveFolder(FString& OutFolder);
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		bool SelectSaveDir(FString& OutFileName);
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		bool GetTimestamp(FString& OutTimestamp);
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		void SetSaveFile(const FString& saveDir);
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		void DoRecording(bool StartBool);
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		void VideoProcessing();
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		bool StillRendering();
	UFUNCTION(BlueprintCallable, Category = "ViewPortRecording")
		void FinishProcessing();

	//delegate
	void CheckRenderingStatus();
	FTimerHandle TimerHandle_RenderingCheck;
	UPROPERTY(BlueprintAssignable, Category = "MyDelegate")
	FMyDelegate ProcessingIsFinished;


protected:
	UGameViewportClient* GameViewportClient;
	UMyGameViewportClient* MyViewportClient;
};
