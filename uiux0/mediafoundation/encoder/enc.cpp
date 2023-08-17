#include <windows.h>
#include <d3d11.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Mftransform.h>
#include <mferror.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")

void EncodeToH264()
{
	int width = 640;
	int height= 480;
	int numerator = 1;
	int denominator = 30;
	int bitrate = 100000;
	int inputBufferSize = 640*480*4;
	int outputBufferSize = 0x100000;

    IMFMediaType *pMediaTypeIn = nullptr;
    IMFMediaType *pMediaTypeOut = nullptr;
    IMFTransform *pEncoder = nullptr;

    IMFMediaBuffer *pBufferIn = nullptr;
    IMFSample* pSampleIn;

    IMFMediaBuffer *pBufferOut = nullptr;
    IMFSample* pSampleOut;

    // 初始化Media Foundation
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr))
    {
        // 错误处理
        return;
    }

    // 创建输入媒体类型
    hr = MFCreateMediaType(&pMediaTypeIn);
    if (FAILED(hr))
    {
        // 错误处理
        goto cleanup;
    }

    // 设置输入媒体类型属性
    hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    // 设置宽度和高度
    hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, width, height);
    // 设置帧率
    hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, numerator, denominator);

    // 创建输出媒体类型
    hr = MFCreateMediaType(&pMediaTypeOut);
    if (FAILED(hr))
    {
        // 错误处理
        goto cleanup;
    }

    // 设置输出媒体类型属性
    hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    // 设置宽度和高度
    hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, width, height);
    // 设置比特率
    hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
    // 设置帧率
    hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, numerator, denominator);

    // 创建H.264编码器
    //hr = MFCreateTransformActivate((IMFActivate**)&pEncoder);
    hr = MFCreateTransformObject(CLSID_CMSH264EncoderMFT, IID_IMFTransform, (void**)&pEncoder);
    if (FAILED(hr))
    {
        // 错误处理
        goto cleanup;
    }

    // 配置编码器输入和输出媒体类型
    hr = pEncoder->SetInputType(0, pMediaTypeIn, 0);
    hr = pEncoder->SetOutputType(0, pMediaTypeOut, 0);

    // 初始化编码器
    hr = pEncoder->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
    hr = pEncoder->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
    hr = pEncoder->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);

    // 创建缓冲区用于存储编码后的数据
    hr = MFCreateMemoryBuffer(outputBufferSize, &pBufferIn);

    // 循环读取并编码每一帧图像
    while (1)
    {
        // 从设备获取图像数据

        // 将图像数据写入输入缓冲区
        hr = pBufferIn->SetCurrentLength(inputBufferSize);
        BYTE* pBufferPointer = nullptr;
        hr = pBufferIn->Lock(&pBufferPointer, nullptr, nullptr);
        // 将图像数据拷贝到pBufferPointer中
        hr = pBufferIn->Unlock();
        hr = pEncoder->ProcessInput(0, pSampleIn, 0);

        // 编码图像数据
        MFT_OUTPUT_STREAM_INFO streamInfo;
        hr = pEncoder->GetOutputStreamInfo(0, &streamInfo);
        //IMFMediaBuffer* pBufferOut = nullptr;
        DWORD outputStatus = 0;
        while (true)
        {
            hr = MFCreateSample(&pSampleOut);
            hr = MFCreateMemoryBuffer(streamInfo.cbSize, &pBufferOut);
            hr = pSampleOut->AddBuffer(pBufferOut);

		MFT_OUTPUT_DATA_BUFFER outputDataBuffer;
		ZeroMemory(&outputDataBuffer, sizeof(MFT_OUTPUT_DATA_BUFFER));
		outputDataBuffer.pSample = pSampleOut;
		outputDataBuffer.dwStreamID = 0;
		outputDataBuffer.dwStatus = 0;
		outputDataBuffer.pEvents = nullptr;
            hr = pEncoder->ProcessOutput(0, 1, &outputDataBuffer, &outputStatus);

            // 如果编码完成，将编码后的数据写入Sink Writer
            if (outputStatus == 0 && pBufferOut != nullptr)
            {
                hr = pSampleOut->ConvertToContiguousBuffer(&pBufferOut);
                pBufferOut->Release();
            }

            // 如果没有更多输出数据
            if (outputStatus != MF_E_TRANSFORM_NEED_MORE_INPUT)
                break;
        }
    }

cleanup:
    // 清理资源
    pMediaTypeIn->Release();
    pMediaTypeOut->Release();
    pEncoder->Release();

    // 关闭Media Foundation
    MFShutdown();
}

int main()
{
    EncodeToH264();

    return 0;
}
