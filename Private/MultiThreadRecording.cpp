// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiThreadRecording.h"
extern "C"
{
#include <libavcodec/codec.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
}


FMultiThreadRecording::FMultiThreadRecording(FIntPoint Viewport, std::string Path, UMyGameViewportClient* GameViewport)
{
    if ((ViewportSize.X != 0 || ViewportSize.Y != 0) && !FilePath.empty() && GameViewport)
    {
        MyViewportClient = GameViewport;
        ViewportSize = Viewport;
        FilePath = Path;
        CurrentThreadActor = GameViewport;
    }
    return;

}

bool FMultiThreadRecording::Init()
{
    bStopThread = false;
    InitCodec();
    return true;
}

uint32 FMultiThreadRecording::Run()
{
    if (MyViewportClient)
    {
        if (!bStopThread)
        {
            while (MyViewportClient->doRecording)
            {
                if (MyViewportClient->Rendering == true)
                {
                    TQueue<TArray<FColor>>& ColorQueue = MyViewportClient->GetColorQueue(); // Get a reference to the ColorQueue in MyGameViewportClient
                    FScopeLock Lock(&MyViewportClient->ColorBufferMutex);
                    if (ColorQueue.Dequeue(ColorBuffer))
                    {
                        Lock.Unlock(); // Unlock the mutex before calling EncodeAndWrite
                        EncodeAndWrite(ColorBuffer);
                    }
                }
                else if (MyViewportClient->Rendering == false)
                {
                    bStopThread = true;

                }

            }
            TidyUp();
            MyViewportClient->FinishedRendering();
            MyViewportClient->ClearThread();
            return 0;
        }
        return 0;

    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Error Processing");
        return 0;
    }
}

void FMultiThreadRecording::Stop()
{

}

//#1 Do this First Before Making Video [Initialize Codec]
void FMultiThreadRecording::InitCodec()
{
    //av_register_all(); //Deprecated
    // FilePath = "E:\\Works\\Output\\xxx.avi"; //Need location
    avformat_alloc_output_context2(&FmtCtx, nullptr, "avi", FilePath.c_str());
    if (!FmtCtx)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "cannot alloc format context");
        return;
    }
    Fmt = FmtCtx->oformat;

    //auto codec_id = AV_CODEC_ID_H264;
    const char codec_name[32] = "h264_nvenc";
    //auto codec = avcodec_find_encoder(codec_id);
    auto codec = avcodec_find_encoder_by_name("h264_nvenc");

    // av_format_set_video_codec(FmtCtx, codec);
    FmtCtx->video_codec = codec;

    if (Fmt->video_codec != AV_CODEC_ID_NONE)
    {

        AddStream(Fmt->video_codec);
    }
    OpenVideo();
    VideoSt.NextPts = 0;
    av_dump_format(FmtCtx, 0, FilePath.c_str(), 1);



    if (!(Fmt->flags & AVFMT_NOFILE))
    {
        auto ret = avio_open(&FmtCtx->pb, FilePath.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, ("Could not open the ret Thing"));
            return;
        }
    }

    auto ret = avformat_write_header(FmtCtx, &Opt);
    if (ret < 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Error occurred when writing header");
        return;
    }

    InLineSize[0] = 4 * VideoSt.Ctx->width;
    SwsCtx = sws_getContext(VideoSt.Ctx->width, VideoSt.Ctx->height, AV_PIX_FMT_RGBA,
        VideoSt.Ctx->width, VideoSt.Ctx->height, VideoSt.Ctx->pix_fmt,
        0, nullptr, nullptr, nullptr);
}

//#1.1 Set Video preference
void FMultiThreadRecording::AddStream(enum AVCodecID CodecID)
{
    VideoCodec = avcodec_find_encoder(CodecID);
    if (!VideoCodec)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not find encoder");
    }


    VideoSt.Stream = avformat_new_stream(FmtCtx, nullptr);
    if (!VideoSt.Stream)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not allocate stream");
    }

    //VideoSt.Stream->id = FmtCtx->nb_streams - 1;
    VideoSt.Ctx = avcodec_alloc_context3(VideoCodec);
    if (!VideoSt.Ctx)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not alloc an encoding context");

    }

    VideoSt.Ctx->codec_id = CodecID;
    VideoSt.Ctx->bit_rate = 8000000; //1080p= 8000000/8 Mbps
    VideoSt.Ctx->width = ViewportSize.X;
    VideoSt.Ctx->height = ViewportSize.Y;
    VideoSt.Ctx->time_base = { 1, 30 }; //1 , FRAMERATE
    VideoSt.Stream->time_base = { 1, 30 };
    VideoSt.Ctx->gop_size = 10; // higher quility
    VideoSt.Ctx->max_b_frames = 1;
    VideoSt.Ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(VideoSt.Ctx->priv_data, "cq", TCHAR_TO_ANSI(*H264Crf), 0);  // change `cq` to `crf` if using libx264
    av_opt_set(VideoSt.Ctx->priv_data, "gpu", TCHAR_TO_ANSI(*DeviceNum), 0); // comment this line if using libx264

    if (FmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
        VideoSt.Ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

//#1.2 Open Video for record
void FMultiThreadRecording::OpenVideo()
{
    auto c = VideoSt.Ctx;
    AVDictionary* opt = nullptr;

    av_dict_copy(&opt, Opt, 0);

    auto ret = avcodec_open2(c, VideoCodec, &opt);
    av_dict_free(&opt);
    if (ret < 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not open video codec");
    }

    AllocPicture();
    if (!VideoSt.Frame)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not allocate video frame");

        return;
    }
    if (avcodec_parameters_from_context(VideoSt.Stream->codecpar, c))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not copy the stream parameters");

    }
}

//#1.3 Set Video Frame preference
void FMultiThreadRecording::AllocPicture()
{
    VideoSt.Frame = av_frame_alloc();
    if (!VideoSt.Frame)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "av_frame_alloc failed");
        return;
    }

    VideoSt.Frame->format = VideoSt.Ctx->pix_fmt;
    VideoSt.Frame->width = ViewportSize.X;
    VideoSt.Frame->height = ViewportSize.Y;

    if (av_frame_get_buffer(VideoSt.Frame, 32) < 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Could not allocate frame data");

    }
}


//#2.1 Set Color To Frame
void FMultiThreadRecording::EncodeAndWrite(TArray<FColor> ColorBuffers)
{
    ColorBuffer = ColorBuffers;
    Pkt = { nullptr };
    Pkt = av_packet_alloc();
    // av_init_packet(&Pkt); //deprecated

    fflush(stdout);
    if (ColorBuffer.Num() <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Need ColorBuffer");
    }

    IMG_Buffer.SetNum(ColorBuffer.Num() * 4);
    uint8* DestPtr = nullptr;
    for (auto i = 0; i < ColorBuffer.Num(); i++)
    {
        DestPtr = &IMG_Buffer[i * 4];
        auto SrcPtr = ColorBuffer[i];
        *DestPtr++ = SrcPtr.R;
        *DestPtr++ = SrcPtr.G;
        *DestPtr++ = SrcPtr.B;
        *DestPtr++ = SrcPtr.A;
    }

    uint8* inData[1] = { IMG_Buffer.GetData() };
    sws_scale(SwsCtx, inData, InLineSize, 0, VideoSt.Ctx->height, VideoSt.Frame->data, VideoSt.Frame->linesize);
    // FString printSws_Scale = FString::Printf(TEXT("%d"), sws_scale(SwsCtx, inData, InLineSize, 0, VideoSt.Ctx->height, VideoSt.Frame->data, VideoSt.Frame->linesize));
    // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Sws_Scale"+printSws_Scale);

    VideoSt.Frame->pts = VideoSt.NextPts++;
    if (FFmpegEncode(VideoSt.Frame) < 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Error encoding frame");
    }
    //else
    //{

    //    FString chheckframe = FString::Printf(TEXT("Error while writing video frame: %d"), VideoSt.Frame->pts);
    //    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, chheckframe);
    //}

    int ret = WriteFrame();
    //if (ret < 0)
    //{
    //    FString ErrorMessage = FString::Printf(TEXT("Error while writing video frame: %d"), ret);
    //    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ErrorMessage);
    //}
    av_packet_unref(Pkt);
}

//#2.2 Check Frame
int FMultiThreadRecording::FFmpegEncode(AVFrame* frame) {
    GotOutput = 0;
    auto ret = avcodec_send_frame(VideoSt.Ctx, frame);
    if (ret < 0 && ret != AVERROR_EOF) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ("error during sending frame, error"));
        return 0;
    }

    ret = avcodec_receive_packet(VideoSt.Ctx, Pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return 0;

    if (ret < 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ("Error during receiving frame"));
        av_packet_unref(Pkt);
        return 0;
    }

    GotOutput = 1;
    return 0;
}

//#2.3 Write Frame
int FMultiThreadRecording::WriteFrame()
{
    if (!VideoSt.Ctx)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "VideoSt.Ctx");

    }
    if (!VideoSt.Stream)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "VideoSt.Ctx");

    }
    av_packet_rescale_ts(Pkt, VideoSt.Ctx->time_base, VideoSt.Stream->time_base);
    Pkt->stream_index = VideoSt.Stream->index;
    return av_interleaved_write_frame(FmtCtx, Pkt);
}

//#3 Flush File
void FMultiThreadRecording::TidyUp()
{
    /* get the delayed frames */
    for (GotOutput = 1; GotOutput; count++)
    {
        fflush(stdout);
        FFmpegEncode(nullptr);
        if (GotOutput)
        {
            auto ret = WriteFrame();
            if (ret < 0)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Error while writing video frame");
            }
            av_packet_unref(Pkt);
        }
    }

    auto ret = av_write_trailer(FmtCtx);
    if (ret < 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "writing trailer error");
    }
    CloseStream();
}

//#3.1 Close Buffer
void FMultiThreadRecording::CloseStream()
{
    avcodec_free_context(&VideoSt.Ctx);
    av_frame_free(&VideoSt.Frame);
    sws_freeContext(SwsCtx);

    if (!(Fmt->flags & AVFMT_NOFILE))
    {
        auto ret = avio_closep(&FmtCtx->pb);
        if (ret < 0)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "avio close failed");
        }
    }
    avformat_free_context(FmtCtx);
}

//#3.2 Write To File
void FMultiThreadRecording::WriteToFile()
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* FileHandle = PlatformFile.OpenWrite(*UEFilePath, true);

    if (FileHandle)
    {
        // Flush the file buffer
        FileHandle->Flush();

        // Close the file handle
        delete FileHandle;
    }
}
