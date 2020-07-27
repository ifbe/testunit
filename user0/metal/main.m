#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
enum VertexAttributes {
	VertexAttributePosition = 0,
	VertexAttributeColor = 1,
};
enum BufferIndex  {
	MeshVertexBuffer = 0,
	FrameUniformBuffer = 1,
};
struct Vertex {
	float position[3];
	unsigned char color[4];
};
NSInteger width = 1024;
NSInteger height = 768;
const int uniformBufferCount = 3;




@interface MyApplicationDelegate : NSObject <NSApplicationDelegate> {
}
@end

@implementation MyApplicationDelegate : NSObject
- (id)init{
NSLog(@"init MyApplicationDelegate");
	self = [super init];
	if(nil != self){
	}
	return self;
}
- (void)windowDidResize:(NSNotification*)noti{
	NSLog(@"windowDidResize");
}
- (void)applicationWillFinishLaunching:(NSNotification*)noti{
	NSLog(@"applicationWillFinishLaunching");
}
- (void)applicationDidFinishLaunching:(NSNotification*)noti{
	NSLog(@"applicationDidFinishLaunching");
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp activateIgnoringOtherApps:YES];
}
- (void)applicationShouldTerminate{
	NSLog(@"applicationShouldTerminate");
}
- (void)applicationWillTerminate{
	NSLog(@"applicationWillTerminate");
}
- (void)applicationDidChangeScreenParameters{
	NSLog(@"applicationDidChangeScreenParameters");
}
- (void)dealloc{
	NSLog(@"free");
	[super dealloc];
}
@end




@interface MyWindowDelegate : NSObject <NSWindowDelegate> {
}
@end

@implementation MyWindowDelegate : NSObject
- (id)init{
NSLog(@"init MyWindowDelegate");
	self = [super init];
	if(nil != self){
	}
	return self;
}
- (void)windowWillBeginSheet:(NSNotification*)noti{
	NSLog(@"windowWillBeginSheet");
}
- (void)windowDidEndSheet:(NSNotification*)noti{
	NSLog(@"windowDidEndSheet");
}
- (void)windowDidResize:(NSNotification*)noti{
	NSLog(@"windowDidResize");
}
- (void)windowWillMiniaturize:(NSNotification*)noti{
	NSLog(@"windowWillMiniaturize");
}
- (void)windowDidMiniaturize:(NSNotification*)noti{
	NSLog(@"windowDidMiniaturize");
}
- (void)windowDidDeminiaturize:(NSNotification*)noti{
	NSLog(@"windowDidDeminiaturize");
}
- (void)windowWillEnterFullScreen:(NSNotification*)noti{
	NSLog(@"windowWillEnterFullScreen");
}
- (void)windowDidEnterFullScreen:(NSNotification*)noti{
	NSLog(@"windowDidEnterFullScreen");
}
- (void)windowWillExitFullScreen:(NSNotification*)noti{
	NSLog(@"windowWillExitFullScreen");
}
- (void)windowDidExitFullScreen:(NSNotification*)noti{
	NSLog(@"windowDidExitFullScreen");
}
- (void)windowWillClose:(NSNotification*)noti{
	NSLog(@"windowWillClose");
	exit(0);
}
@end




@interface MyView : MTKView
@end

@implementation MyView {
	id<MTLDevice> device;
	id <MTLRenderPipelineState> _pipelineState;
	id <MTLDepthStencilState> _depthState;

	id <MTLLibrary> _library;
	id <MTLCommandQueue> _commandQueue;

	id <MTLBuffer> _vertexBuffer;
	long frame;
}
- (id)initWithFrame:(CGRect)inFrame {
NSLog(@"initWithFrame");
	device = MTLCreateSystemDefaultDevice();

	self = [super initWithFrame:inFrame device:device];
	if (self) {
		[self setup];
	}
	return self;
}
- (void)setup {
NSLog(@"setup");
	// Set view settings.
	self.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
	self.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

	// Load shaders.
	NSError *error = nil;
	_library = [self.device newLibraryWithFile: @"shaders.metallib" error:&error];
	if (!_library) {
		NSLog(@"Failed to load library. error %@", error);
		exit(0);
	}
	id <MTLFunction> vertFunc = [_library newFunctionWithName:@"vert"];
	id <MTLFunction> fragFunc = [_library newFunctionWithName:@"frag"];

	// Create depth state.
	MTLDepthStencilDescriptor *depthDesc = [MTLDepthStencilDescriptor new];
	depthDesc.depthCompareFunction = MTLCompareFunctionLess;
	depthDesc.depthWriteEnabled = YES;
	_depthState = [self.device newDepthStencilStateWithDescriptor:depthDesc];

	// Create vertex descriptor.
	MTLVertexDescriptor *vertDesc = [MTLVertexDescriptor new];
	vertDesc.attributes[VertexAttributePosition].format = MTLVertexFormatFloat3;
	vertDesc.attributes[VertexAttributePosition].offset = 0;
	vertDesc.attributes[VertexAttributePosition].bufferIndex = MeshVertexBuffer;
	vertDesc.attributes[VertexAttributeColor].format = MTLVertexFormatUChar4;
	vertDesc.attributes[VertexAttributeColor].offset = 4*3;
	vertDesc.attributes[VertexAttributeColor].bufferIndex = MeshVertexBuffer;
	vertDesc.layouts[MeshVertexBuffer].stride = 4*4;
	vertDesc.layouts[MeshVertexBuffer].stepRate = 1;
	vertDesc.layouts[MeshVertexBuffer].stepFunction = MTLVertexStepFunctionPerVertex;

	// Create pipeline state.
	MTLRenderPipelineDescriptor *pipelineDesc = [MTLRenderPipelineDescriptor new];
	pipelineDesc.sampleCount = self.sampleCount;
	pipelineDesc.vertexFunction = vertFunc;
	pipelineDesc.fragmentFunction = fragFunc;
	pipelineDesc.vertexDescriptor = vertDesc;
	pipelineDesc.colorAttachments[0].pixelFormat = self.colorPixelFormat;
	pipelineDesc.depthAttachmentPixelFormat = self.depthStencilPixelFormat;
	pipelineDesc.stencilAttachmentPixelFormat = self.depthStencilPixelFormat;
	_pipelineState = [self.device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
	if (!_pipelineState) {
		NSLog(@"Failed to create pipeline state, error %@", error);
		exit(0);
	}

	// Create vertices.
	struct Vertex verts[] = {
		{{-0.5,-0.5, 0}, {255, 0, 0, 255}},
		{{   0, 0.5, 0}, {0, 255, 0, 255}},
		{{0.5, -0.5, 0}, {0, 0, 255, 255}}
	};
	_vertexBuffer = [self.device
		newBufferWithBytes:verts
		length:sizeof(verts)
		options:MTLResourceStorageModeShared
	];

	// Create command queue
	_commandQueue = [self.device newCommandQueue];
}
- (void)drawRect:(CGRect)rect
{
NSLog(@"drawRect");
	// Create a command buffer.
	id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];

	MTLViewport vp = {
		.originX = 0.0,
		.originY = 0.0,
		.width = self.drawableSize.width,
		.height= self.drawableSize.height,
		.znear= 0.0,
		.zfar = 1.0
	};

	// Encode render command.
	id <MTLRenderCommandEncoder> encoder =
		[commandBuffer renderCommandEncoderWithDescriptor:self.currentRenderPassDescriptor];
	[encoder setViewport:vp];
	[encoder setDepthStencilState:_depthState];
	[encoder setRenderPipelineState:_pipelineState];
	[encoder setVertexBuffer:_vertexBuffer offset:0 atIndex:MeshVertexBuffer];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
	[encoder endEncoding];

	[commandBuffer presentDrawable:self.currentDrawable];
	[commandBuffer commit];

	[super drawRect:rect];
}
-(void)mouseDown:(NSEvent *)event{
NSLog(@"mouseDown");
}
@end




int main(int argc, const char* argv[])
{
	//app
	[NSAutoreleasePool new];
	[NSApplication sharedApplication];

	MyApplicationDelegate* applicationdelegate = [[[MyApplicationDelegate alloc] init] autorelease];
	[NSApp setDelegate:applicationdelegate];
	[NSApp finishLaunching];


	//menu
	id appName = [[NSProcessInfo processInfo] processName];
	id quitTitle = [@"Quit " stringByAppendingString:appName];
	id quitMenuItem = [[[NSMenuItem alloc]
		initWithTitle:quitTitle
		action:@selector(terminate:)
		keyEquivalent:@"q"] autorelease];

	id appMenu = [[NSMenu new] autorelease];
	[appMenu addItem:quitMenuItem];

	id appMenuItem = [[NSMenuItem new] autorelease];
	[appMenuItem setSubmenu:appMenu];

	id menubar = [[NSMenu new] autorelease];
	[menubar addItem:appMenuItem];
	[NSApp setMainMenu:menubar];


	//window
	NSRect windowRect = NSMakeRect(0, 0, 1024, 768);
	NSUInteger windowStyle =
		NSWindowStyleMaskTitled |
		NSWindowStyleMaskClosable |
		NSWindowStyleMaskResizable |
		NSWindowStyleMaskMiniaturizable;
	id window = [[[NSWindow alloc]
		initWithContentRect:windowRect
		styleMask:windowStyle
		backing:NSBackingStoreBuffered
		defer:NO]
	autorelease];
	//[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[window setTitle:appName];
	[window makeKeyAndOrderFront:nil];

	MyWindowDelegate* windowdelegate = [[[MyWindowDelegate alloc] init] autorelease];
	[window setDelegate:windowdelegate];


	//view
	MyView* myview = [[[MyView alloc] initWithFrame:windowRect] autorelease];
	[window setContentView:myview];


	//event
	//[NSApp run];
	while(1){
		NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES ];
		if(nil == event)continue;

		switch ([event type]) {
		case NSEventTypeLeftMouseDown:
		case NSEventTypeOtherMouseDown:
		case NSEventTypeRightMouseDown:
		case NSEventTypeLeftMouseUp:
		case NSEventTypeOtherMouseUp:
		case NSEventTypeRightMouseUp:
		case NSEventTypeLeftMouseDragged:
		case NSEventTypeRightMouseDragged:
		case NSEventTypeOtherMouseDragged:
		case NSEventTypeMouseMoved:
		case NSEventTypeScrollWheel:
		case NSEventTypeKeyDown:
		case NSEventTypeKeyUp:
		case NSEventTypeFlagsChanged:
		default:break;
		}
		[NSApp sendEvent:event];
	}

	return 0;
}
