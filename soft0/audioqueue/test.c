#include "AudioToolbox/AudioToolbox.h"
#define PI 3.141592653
#define tau PI*2


typedef struct {
	int count;
}PhaseBlah;
static PhaseBlah phase = {0};
static float freq[12] = {440, 466, 494, 523, 554, 587, 622, 659, 698.5, 740, 784, 830.6};


void callback (void *ptr, AudioQueueRef aq, AudioQueueBufferRef buf_ref)
{
	OSStatus status;
	PhaseBlah *p = ptr;
	AudioQueueBuffer *buf = buf_ref;
	int nsamp = buf->mAudioDataByteSize / 2;
	short *samp = buf->mAudioData;
	int ii;
	printf ("Callback! nsamp: %d\n", nsamp);
	for (ii = 0; ii < nsamp; ii++) {
		samp[ii] = (int) (30000.0 * sin(ii*freq[p->count]*tau/44100.0));
		//printf("phase: %.3f\n", p->phase);
	}
	p->count = (p->count+1)%12;
	status = AudioQueueEnqueueBuffer(aq, buf_ref, 0, NULL);
	printf ("Enqueue status: %d\n", status);
}


int main (int argc, char *argv[])
{
	AudioStreamBasicDescription fmt = { 0 };
	fmt.mSampleRate = 44100;
	fmt.mFormatID = kAudioFormatLinearPCM;
	fmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
	fmt.mFramesPerPacket = 1;
	fmt.mChannelsPerFrame = 1; // 2 for stereo
	fmt.mBytesPerPacket = fmt.mBytesPerFrame = 2; // x2 for stereo
	fmt.mBitsPerChannel = 16;

	AudioQueueRef aq;
	OSStatus status;
	status = AudioQueueNewOutput(&fmt, callback, &phase, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &aq);
	if (status == kAudioFormatUnsupportedDataFormatError) puts ("oops!");
	else printf("NewOutput status: %d\n", status);


	AudioQueueBufferRef buf_ref;
	status = AudioQueueAllocateBuffer(aq, 2*44100, &buf_ref);
	printf ("Allocate status: %d\n", status);
	AudioQueueBuffer *buf = buf_ref;
	printf ("buf: %p, data: %p, len: %d\n", buf, buf->mAudioData, buf->mAudioDataByteSize);
	buf->mAudioDataByteSize = 2*44100;
	callback (&phase, aq, buf_ref);


	AudioQueueBufferRef buf_ref2;
	status = AudioQueueAllocateBuffer (aq, 2*44100, &buf_ref2);
	printf ("Allocate2 status: %d\n", status);
	buf = buf_ref2;
	buf->mAudioDataByteSize = 2*44100;
	callback (&phase, aq, buf_ref2);


	status = AudioQueueSetParameter(aq, kAudioQueueParam_Volume, 0.5);
	printf ("Volume status: %d\n", status);

	status = AudioQueueStart(aq, NULL);
	printf ("Start status: %d\n", status);

	while(phase.count < 15){
		CFRunLoopRunInMode(
			kCFRunLoopDefaultMode,
			0.25, // seconds
			false // don't return after source handled
		);
	}
/*
	usleep(10*1000*1000);
	AudioQueueStop(aq, 0);
*/
	return 0;
}
