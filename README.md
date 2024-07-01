# UnrealScreenRecoder
The Unreal Screen Recording Plugin is a powerful tool designed to capture and convert screen recordings directly within Unreal Engine using the FFMPEG library. This plugin simplifies the process of recording your game or application screen and converting it to .avi video format.

ตัวอัด Video ใช้ FFMPEG 
{H264_nvenc ใช้ได้แค่ Nvidia Card}
1.ตั้ง Game Viewport Client Class ใน Project Settings เป็น MyGameViewportClient

2.เพิ่ม Component : MyRecordingManager เพื่อใช้ควบคุมการอัด Video

วิธีการอัดวิดีโอ
1. Cast ไปยัง MyGameViewport เพื่อตั้งค่าจุดเซฟวิดีโอ


2.เริ่มอัดวิดีโอ
-Node Still Rendering check ว่า Video กำลังประมวลผลอยู่
-Do Recording :True  เริ่ม Capture Screen
-Video Processing ใช้รูปที่จับมาทำเป็น Video


3.Processing Frame 
-Do Recording :False หยุด Capture Screen รวมถึงการ Processing



4.Delegatedaw
-หลังจากประมวลผลเสร็จแล้วจะเรียก Delegate ใน C++

