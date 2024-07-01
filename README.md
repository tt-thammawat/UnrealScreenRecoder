# UnrealScreenRecoder
The Unreal Screen Recording Plugin is a powerful tool designed to capture and convert screen recordings directly within Unreal Engine using the FFMPEG library. This plugin simplifies the process of recording your game or application screen and converting it to .avi video format.


ตัวอัด Video ใช้ FFMPEG 
{H264_nvenc ใช้ได้แค่ Nvidia Card}


1.ตั้ง Game Viewport Client Class ใน Project Settings เป็น MyGameViewportClient


![msedge_rVXevzZkZh](https://github.com/tt-thammawat/UnrealScreenRecoder/assets/155699388/93212aa7-b7cc-4338-8869-6775fc5e3828)


2.เพิ่ม Component : MyRecordingManager เพื่อใช้ควบคุมการอัด Video

![msedge_SVwM4byi0f](https://github.com/tt-thammawat/UnrealScreenRecoder/assets/155699388/f8313005-5fb4-4456-a393-234d9362ce43)


วิธีการอัดวิดีโอ
1. Cast ไปยัง MyGameViewport เพื่อตั้งค่าจุดเซฟวิดีโอ

![msedge_MbIU1kkDDu](https://github.com/tt-thammawat/UnrealScreenRecoder/assets/155699388/19a4ae22-0599-4a08-9606-8e15412dde1b)


2.เริ่มอัดวิดีโอ
-Node Still Rendering check ว่า Video กำลังประมวลผลอยู่
-Do Recording :True  เริ่ม Capture Screen
-Video Processing ใช้รูปที่จับมาทำเป็น Video


![msedge_XxCKj6BVm3](https://github.com/tt-thammawat/UnrealScreenRecoder/assets/155699388/286b0779-e6dc-4df0-b1af-40e1678c348d)


3.Processing Frame 
-Do Recording :False หยุด Capture Screen รวมถึงการ Processing


![msedge_o97r9OrLwz](https://github.com/tt-thammawat/UnrealScreenRecoder/assets/155699388/589645a4-29aa-48b2-a4eb-6538afce3bc3)


4.Delegatedaw
-หลังจากประมวลผลเสร็จแล้วจะเรียก Delegate ใน C++

![msedge_O9cqcarmse](https://github.com/tt-thammawat/UnrealScreenRecoder/assets/155699388/0dffe596-8556-4c32-9330-29c1e234c55a)

