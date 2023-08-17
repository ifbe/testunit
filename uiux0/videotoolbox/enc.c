#include <stdio.h>
#include <stdlib.h>
#include <CoreMediaIO/CMIOHardware.h>
#include <VideoToolbox/VideoToolbox.h>

// 编码器回调函数
void MyCompressionOutputCallback(void* outputCallbackRefCon, void* sourceFrameRefCon, OSStatus status,
                                 VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer)
{
    if (status != noErr) {
        // 错误处理
        return;
    }
    
    // 处理编码后的CMSampleBuffer
    // ...
}

void EncodeToH264()
{
    CMVideoFormatDescriptionRef outputFormatDesc = NULL;
    VTCompressionSessionRef compressionSession = NULL;
    
    // 创建帧描述对象
    CMVideoFormatDescriptionCreate(NULL, kCMVideoCodecType_H264, width, height, NULL, &outputFormatDesc);
    
    // 创建压缩会话
    VTCompressionSessionCreate(NULL, width, height, kCMVideoCodecType_H264, NULL, NULL, NULL, MyCompressionOutputCallback, NULL, &compressionSession);
    
    // 配置压缩会话
    VTSessionSetProperty(compressionSession, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
    VTSessionSetProperty(compressionSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Baseline_AutoLevel);
    VTSessionSetProperty(compressionSession, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
    
    // 开始编码
    VTCompressionSessionPrepareToEncodeFrames(compressionSession);
    
    while (/* 读取到图像数据 */) {
        // 从设备获取图像数据
        
        // 创建CMSampleBuffer
        CMSampleBufferRef sampleBuffer = NULL;
        CMBlockBufferRef blockBuffer = NULL;
        
        // 创建CMBlockBuffer
        CMBlockBufferCreateWithMemoryBlock(NULL, pixelBufferBaseAddress, pixelBufferSize, kCFAllocatorNull, NULL, 0, pixelBufferSize, 0, &blockBuffer);
        
        // 创建CMSampleBuffer
        CMSampleBufferCreate(NULL, blockBuffer, true, NULL, NULL, outputFormatDesc, 1, 0, NULL, 0, NULL, &sampleBuffer);
        
        // 编码CMSampleBuffer
        VTCompressionSessionEncodeFrame(compressionSession, sampleBuffer, kCMTimeInvalid, kCMTimeInvalid, NULL, NULL, NULL);
        
        // 释放资源
        CFRelease(blockBuffer);
        CFRelease(sampleBuffer);
    }
    
    // 停止编码
    VTCompressionSessionCompleteFrames(compressionSession, true, NULL);
    
    // 清理资源
    CFRelease(outputFormatDesc);
    VTCompressionSessionInvalidate(compressionSession);
    CFRelease(compressionSession);
}

int main()
{
    EncodeToH264();
    
    return 0;
}
