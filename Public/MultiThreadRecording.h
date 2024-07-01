// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/RunnableThread.h"
#include "MyGameViewportClient.h"
#include "HAL/Platform.h"
#include <mutex>
#include <string>
extern "C"
{
#include <libavcodec/codec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <MyGameViewportClient.h>

}

/**
 *
 */

class FRunnableThread;
class UMyGameViewportClient;

class MYRECORDING_API FMultiThreadRecording : public FRunnable
{

public:
	//Thread
	UMyGameViewportClient* CurrentThreadActor;
	FRunnableThread* Thread;
	bool bStopThread;

	//Struct
	struct OutputStream
	{
		AVStream* Stream;
		AVCodecContext* Ctx;

		int64_t NextPts;

		AVFrame* Frame;

		struct SwsContext* SwsCtx;
	};


	//Ref Value Need to get Viewport size and ColorSet of the image from MyGameViewportClient
public:
	//1.InitCodec:
	UPROPERTY(Config)
		FString H264Crf;
	UPROPERTY(Config)
		FString DeviceNum;

	/*Casting*/
	UMyGameViewportClient* MyViewportClient;


	/*ViewPort Size*/
	FIntPoint ViewportSize;

	/*File Path*/
	std::string FilePath = "C:\\temp\\record.avi";
	FString UEFilePath = "C:/temp/record.avi";

	/*FFMPEG*/
	AVFormatContext* FmtCtx;
	AVOutputFormat* Fmt;
	OutputStream VideoSt = { 0 };
	AVDictionary* Opt = nullptr;
	SwsContext* SwsCtx;
	AVCodec* VideoCodec;
	int InLineSize[1];

	//2.Make Video From ColorData
	TArray<FColor> ColorBuffer;
	TArray<uint8> IMG_Buffer;

	AVPacket* Pkt;
	int GotOutput;

	//3.Flush Record
	int count;

	//Function
public:
	// Sets default values for this actor's properties
	FMultiThreadRecording(FIntPoint Viewport, std::string Path, UMyGameViewportClient* GameViewport);

	virtual bool Init();

	virtual uint32 Run();

	virtual void Stop();

	//1.InitCodec:
	void InitCodec();

	void OpenVideo();

	void AddStream(AVCodecID CodecID);

	void AllocPicture();

	//2.Make Video From ColorData

	void EncodeAndWrite(TArray<FColor> ColorBuffers);

	int FFmpegEncode(AVFrame* frame);

	int WriteFrame();


	//3.Flush Record
	void TidyUp();

	void CloseStream();

	void WriteToFile();

};