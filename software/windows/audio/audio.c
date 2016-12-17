#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")
#define u64 unsigned long long
#define u32 unsigned int
//
#define bufsize 1024*1024	//每次开辟10k的缓存存储录音数据
BYTE pBuffer1[bufsize];		//采集音频时的数据缓存
FILE* pcmfile;  //音频文件
//
HWAVEIN hWaveIn;			//输入设备
HWAVEOUT hwo;
//
WAVEFORMATEX fmt;		//采集音频的格式，结构体
WAVEHDR head;				//采集音频时包含数据缓存的结构体




int record()
{
    HANDLE          wait;
    fmt.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM
    fmt.nSamplesPerSec = 48000;//采样率，16000次/秒
    fmt.wBitsPerSample = 16;//采样比特，16bits/次
    fmt.nChannels = 1;//采样声道数，2声道
    fmt.nAvgBytesPerSec = 16000;//每秒的数据率，就是每秒能采集多少字节的数据
    fmt.nBlockAlign = 2;//一个块的大小，采样bit的字节数乘以声道数
    fmt.cbSize = 0;//一般为0

    wait = CreateEvent(NULL, 0, 0, NULL);
    //使用waveInOpen函数开启音频采集
    waveInOpen(&hWaveIn, WAVE_MAPPER, &fmt,(DWORD_PTR)wait, 0L, CALLBACK_EVENT);

    //建立两个数组（这里可以建立多个数组）用来缓冲音频数据
    int i = 10;
    fopen_s(&pcmfile, "test.pcm", "wb");
    while (i--)//录制20左右秒声音，结合音频解码和网络传输可以修改为实时录音播放的机制以实现对讲功能
    {
        head.lpData = (LPSTR)pBuffer1;
        head.dwBufferLength = bufsize;
        head.dwBytesRecorded = 0;
        head.dwUser = 0;
        head.dwFlags = 0;
        head.dwLoops = 1;
        waveInPrepareHeader(hWaveIn, &head, sizeof(WAVEHDR));//准备一个波形数据块头用于录音
        waveInAddBuffer(hWaveIn, &head, sizeof (WAVEHDR));//指定波形数据块为录音输入缓存
        waveInStart(hWaveIn);//开始录音
        Sleep(1000);//等待声音录制1s
        waveInReset(hWaveIn);//停止录音
        fwrite(pBuffer1, 1, head.dwBytesRecorded, pcmfile);
        printf("%ds\n", i);
    }
    fclose(pcmfile);

    waveInClose(hWaveIn);
    return 0;
}




static void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD dwInstance, DWORD dw1, DWORD dw2)//回调函数
{
    switch (uMsg)
    {
        case WOM_DONE://上次缓存播放完成,触发该事件
        {
			head.dwBufferLength = 0;
            printf("done\n");
        }
    }
}
void play(char* name)
{
	int ret;
    fmt.wFormatTag = WAVE_FORMAT_PCM;//设置波形声音的格式
    fmt.nChannels = 1;//设置音频文件的通道数量
    fmt.nSamplesPerSec = 48000;//设置每个声道播放和记录时的样本频率
    fmt.nAvgBytesPerSec = 16000;//设置请求的平均数据传输率,单位byte/s。这个值对于创建缓冲大小是很有用的
    fmt.nBlockAlign = 2;//以字节为单位设置块对齐
    fmt.wBitsPerSample = 16;
    fmt.cbSize = 0;//额外信息的大小

    fopen_s(&pcmfile, name, "rb");//打开文件
    waveOutOpen(&hwo, WAVE_MAPPER, &fmt, (u64)WaveCallback, 0L, CALLBACK_FUNCTION);

    ret = fread(pBuffer1, 1, bufsize, pcmfile);
    head.dwLoops = 0L;
    head.lpData = pBuffer1;
    head.dwBufferLength = ret;
    head.dwFlags = 0L;
    waveOutPrepareHeader(hwo, &head, sizeof(WAVEHDR));//准备一个波形数据块用于播放
    waveOutWrite(hwo, &head, sizeof(WAVEHDR));//在音频媒体中播放第二个参数指定的数据，也相当于开启一个播放区的意思

    while (head.dwBufferLength != 0)//如果文件还在没播放完则等待500ms
    {
        Sleep(500);
    }
    waveOutUnprepareHeader(hwo, &head, sizeof(WAVEHDR));

    fclose(pcmfile);//关闭文件
    return;
}




void main(int argc, char** argv)
{
	if(argc == 1)record();
	else play("test.pcm");
}