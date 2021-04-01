#include <AVFoundation/AVFoundation.h>




@interface MyDelegate : NSObject
{
}
@end

@implementation MyDelegate : NSObject
- (void)  captureOutput:(AVCaptureOutput *)captureOutput
  didOutputSampleBuffer:(CMSampleBufferRef)videoFrame
		 fromConnection:(AVCaptureConnection *)connection
{
	NSLog(@"captureOutput");

	// Get a CMSampleBuffer's Core Video image buffer for the media data
	CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(videoFrame);
/*
	// Lock the base address of the pixel buffer
	CVPixelBufferLockBaseAddress(imageBuffer, 0);

	// Get the number of bytes per row for the pixel buffer
	void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);

	// Get the number of bytes per row for the pixel buffer
	size_t bytesTotal = CVPixelBufferGetDataSize(imageBuffer);

	// Get the pixel buffer width and height

	NSLog(@"w=%zu, h=%zu, bytesPerRow=%zu, bytesTotal=%zu", width, height, bytesPerRow, bytesTotal);
*/
	size_t width = CVPixelBufferGetWidth(imageBuffer);
	size_t width_y = CVPixelBufferGetWidthOfPlane(imageBuffer,0);
	size_t width_u = CVPixelBufferGetWidthOfPlane(imageBuffer,1);
	size_t width_v = CVPixelBufferGetWidthOfPlane(imageBuffer,1);
	NSLog(@"width=%zu: %zu,%zu,%zu", width, width_y, width_u, width_v);

	size_t height = CVPixelBufferGetHeight(imageBuffer);
	size_t height_y = CVPixelBufferGetHeightOfPlane(imageBuffer,0);
	size_t height_u = CVPixelBufferGetHeightOfPlane(imageBuffer,1);
	size_t height_v = CVPixelBufferGetHeightOfPlane(imageBuffer,2);
	NSLog(@"height=%zu: %zu,%zu,%zu", height, height_y, height_u, height_v);

	size_t bpr = CVPixelBufferGetBytesPerRow(imageBuffer);
	size_t bpr_y = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);
	size_t bpr_u = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 1);
	size_t bpr_v = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 2);
	NSLog(@"bpr=%zu: %zu,%zu,%zu", bpr, bpr_y, bpr_u, bpr_v);

	OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);
	NSLog(@"fmt=%.4s", (char*)&pixelFormat);
/*
	void* base_y = CVPixelBufferGetBaseAddressOfPlane(ypane, 0);
	void* base_u = CVPixelBufferGetBaseAddressOfPlane(ypane, 1);
	void* base_v = CVPixelBufferGetBaseAddressOfPlane(ypane, 2);
	NSLog(@"base=%p,%p,%p", base_y, base_u, base_v);
*/
}
@end




int main()
{
	AVCaptureDeviceDiscoverySession* video_discovery = [AVCaptureDeviceDiscoverySession
		discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera]
		mediaType:AVMediaTypeVideo
		position:AVCaptureDevicePositionUnspecified
	];
	AVCaptureDeviceDiscoverySession* audio_discovery = [AVCaptureDeviceDiscoverySession
		discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInMicrophone]
		mediaType:AVMediaTypeAudio
		position:AVCaptureDevicePositionUnspecified
	];/*
	AVCaptureDeviceDiscoverySession* muxed_discovery = [AVCaptureDeviceDiscoverySession
		discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeExternalUnknown]
		mediaType:AVMediaTypeMuxed
		position:AVCaptureDevicePositionUnspecified
	];*/

	NSArray* video_devices  = video_discovery.devices;
	NSArray* audio_devices  = audio_discovery.devices;
	//NSArray* muxed_devices  = muxed_discovery.devices;

	AVCaptureDevice* chosen = 0;
	for (AVCaptureDevice* device in video_devices) {
		const char *name = [[device localizedName] UTF8String];
		int index		= [video_devices indexOfObject:device];
		NSLog(@"video[%d] %s\n", index, name);
		chosen = device;
	}
	for (AVCaptureDevice* device in audio_devices) {
		const char *name = [[device localizedName] UTF8String];
		int index		= [audio_devices indexOfObject:device];
		NSLog(@"audio[%d] %s\n", index, name);
	}/*
	for (AVCaptureDevice* device in muxed_devices) {
		const char *name = [[device localizedName] UTF8String];
		int index		= [muxed_devices indexOfObject:device];
		NSLog(@"muxed[%d] %s\n", index, name);
	}*/
	if(0 == chosen)return 0;

	switch([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]){
	case AVAuthorizationStatusAuthorized:
		// Has access
		NSLog(@"Authorized\n");
		break;
	case AVAuthorizationStatusDenied:
		// No access granted
		NSLog(@"Denied\n");
		break;
	case AVAuthorizationStatusRestricted:
		// Microphone disabled in settings
		NSLog(@"Restricted\n");
		break;
	case AVAuthorizationStatusNotDetermined:
		// Didn't request access yet
		NSLog(@"NotDetermined\n");

//		[AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
//		}];
		break;
	default:
		NSLog(@"default\n");
		break;
	}

	NSError *err;
	AVCaptureDeviceInput* input = [AVCaptureDeviceInput deviceInputWithDevice:chosen error:&err];
	NSLog(@"%ld\n",[err code]);

	AVCaptureVideoDataOutput* output = [[AVCaptureVideoDataOutput alloc]init];
	NSLog(@"alive\n");

	id delegate = [[MyDelegate alloc] init];
	dispatch_queue_t queue = dispatch_queue_create("avf_queue", NULL);
	[output setSampleBufferDelegate:delegate queue:queue];

	AVCaptureSession* session = [[AVCaptureSession alloc] init];
	session.sessionPreset = AVCaptureSessionPreset640x480;
	if([session canAddInput:input]){
		[session addInput:input];
	}
	if([session canAddOutput:output]){
		[session addOutput:output];
	}
	[session startRunning];

	while(1);
	return 0;
}
