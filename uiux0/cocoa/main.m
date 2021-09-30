#import <Cocoa/Cocoa.h>
UInt8* data;
NSInteger width = 1024;
NSInteger height = 768;
static void CreateApplicationMenus(void)
{
	NSString *appName;
	NSString *title;
	NSMenu *appleMenu;
	NSMenu *serviceMenu;
	NSMenu *windowMenu;
	NSMenuItem *menuItem;
	NSMenu *mainMenu;
	if (NSApp == nil)return;

	mainMenu = [[NSMenu alloc] init];
    
	/* Create the main menu bar */
	[NSApp setMainMenu:mainMenu];

	[mainMenu release];  /* we're done with it, let NSApp own it. */
	mainMenu = nil;

	/* Create the application menu */
	appName = @"42";
	appleMenu = [[NSMenu alloc] initWithTitle:@""];
    
	/* Add menu items */
	title = [@"About " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
	[appleMenu addItem:[NSMenuItem separatorItem]];
	[appleMenu addItemWithTitle:@"Preferences…" action:nil keyEquivalent:@","];
	[appleMenu addItem:[NSMenuItem separatorItem]];
    
	serviceMenu = [[NSMenu alloc] initWithTitle:@""];
	menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Services" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:serviceMenu];
    
	[NSApp setServicesMenu:serviceMenu];
	[serviceMenu release];
    
	[appleMenu addItem:[NSMenuItem separatorItem]];
    
	title = [@"Hide " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];
    
	menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:(NSEventModifierFlagOption|NSEventModifierFlagCommand)];
    
	[appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
    
	[appleMenu addItem:[NSMenuItem separatorItem]];
    
	title = [@"Quit " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
    
	/* Put menu into the menubar */
	menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:appleMenu];
	[[NSApp mainMenu] addItem:menuItem];
	[menuItem release];
    
	/* Tell the application object that this is now the application menu */
	//[NSApp setAppleMenu:appleMenu];
	[appleMenu release];
    
    
	/* Create the window menu */
	windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    
	/* Add menu items */
	[windowMenu addItemWithTitle:@"Close" action:@selector(performClose:) keyEquivalent:@"w"];
	[windowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
	[windowMenu addItemWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];
	
	/* Add the fullscreen toggle menu option, if supported */
	if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6) {
	    /* Cocoa should update the title to Enter or Exit Full Screen automatically.
	     * But if not, then just fallback to Toggle Full Screen.
	     */
	    menuItem = [[NSMenuItem alloc] initWithTitle:@"Toggle Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"];
	    [menuItem setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
	    [windowMenu addItem:menuItem];
	    [menuItem release];
	}
    
	/* Put menu into the menubar */
	menuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:windowMenu];
	[[NSApp mainMenu] addItem:menuItem];
	[menuItem release];
    
	/* Tell the application object that this is now the window menu */
	[NSApp setWindowsMenu:windowMenu];
	[windowMenu release];
}




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
/*
	//menu
	id quitMenuItem = [[[NSMenuItem alloc]
		initWithTitle:@"quit"
		action:@selector(terminate:)
		keyEquivalent:@"q"]
	autorelease];
	id appMenu = [[[NSMenu alloc] initWithTitle:@"1111"] autorelease];
	[appMenu addItem:quitMenuItem];
	id appMenuItem = [[NSMenuItem new] autorelease];
	[appMenuItem setSubmenu:appMenu];

	id quitMenuItem2 = [[[NSMenuItem alloc]
		initWithTitle:@"quit"
		action:@selector(terminate:)
		keyEquivalent:@"q"]
	autorelease];
	id quitMenuItem21 = [[[NSMenuItem alloc]
		initWithTitle:@"quit"
		action:@selector(terminate:)
		keyEquivalent:@"q"]
	autorelease];
	id appMenu2 = [[[NSMenu alloc] initWithTitle:@"2222"] autorelease];
	[appMenu2 addItem:quitMenuItem2];
	[appMenu2 addItem:quitMenuItem21];
	id appMenuItem2 = [[NSMenuItem new] autorelease];
	[appMenuItem2 setSubmenu:appMenu2];

	id quitMenuItem3 = [[[NSMenuItem alloc]
		initWithTitle:@"quit"
		action:@selector(terminate:)
		keyEquivalent:@"q"]
	autorelease];
	id appMenu3 = [[[NSMenu alloc] initWithTitle:@"3333"] autorelease];
	[appMenu3 addItem:quitMenuItem3];
	id appMenuItem3 = [[NSMenuItem new] autorelease];
	[appMenuItem3 setSubmenu:appMenu3];

	id mainmenu = [[[NSMenu alloc] initWithTitle:@"main"] autorelease];
	[mainmenu addItem:appMenuItem];
	[mainmenu addItem:appMenuItem2];
	[mainmenu addItem:appMenuItem3];

	[NSApp setMainMenu:mainmenu];
*/
	CreateApplicationMenus();
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
- (BOOL)canBecomeKeyWindow{
	NSLog(@"canBecomeKeyWindow");
	return YES;
}

- (BOOL)canBecomeMainWindow{
	NSLog(@"canBecomeMainWindow");
	return YES;
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




@interface MyView : NSView {
}
@end

@implementation MyView
- (id)initWithFrame:(NSRect)rect{
NSLog(@"initWithFrame");
	self = [super initWithFrame:rect];
	if(nil != self){
	}
	return self;
}
- (void)drawRect:(NSRect)rect
{
NSLog(@"drawRect");
	//[[NSColor redColor] setFill];
	//NSRectFill(rect);
	//[super drawRect:rect];

	//Fill pixel buffer with color data
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++) {

			int index = 4*(x+y*width);
			data[index+0] = x/4;
			data[index+1] = y/4;
			data[index+2] = 0;
			data[index+3] = 255;
		}
	}

	// Create a CGImage with the pixel data
	NSInteger dataLength = width * height * 4;
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataLength, NULL);
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
	CGImageRef image = CGImageCreate(
		width, height, 8, 32, width * 4, colorspace,
		kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast,
		provider, NULL, true, kCGRenderingIntentDefault
	);

	//draw
	CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];
	CGRect renderRect = CGRectMake(0., 0., 1024, 768);
	CGContextDrawImage(ctx, renderRect, image);

	//Clean up
	CGColorSpaceRelease(colorspace);
	CGDataProviderRelease(provider);
	CGImageRelease(image);
}
-(void)mouseDown:(NSEvent *)event{
NSLog(@"mouseDown");
}
-(void)mouseUp:(NSEvent *)event{
NSLog(@"mouseUp");
}
-(void)keyDown:(NSEvent *)event{
NSLog(@"keyDown");
}
-(void)keyUp:(NSEvent *)event{
NSLog(@"keyUp");
}
@end




int main(int argc, const char* argv[])
{
	//app
	[NSAutoreleasePool new];
	[NSApplication sharedApplication];

	MyApplicationDelegate* appdelegate = [[[MyApplicationDelegate alloc] init] autorelease];
	[NSApp setDelegate:appdelegate];
	[NSApp finishLaunching];


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
	[window setTitle:@"cocoa"];
	[window makeKeyAndOrderFront:nil];

	MyWindowDelegate* windelegate = [[[MyWindowDelegate alloc] init] autorelease];
	[window setDelegate:windelegate];


	//view
	data = (UInt8*)malloc(sizeof(UInt8) * 4 * width * height);
	if(0 == data)return 0;

	MyView* myview = [[[MyView alloc] initWithFrame:windowRect] autorelease];
	[window setContentView:myview];


	//event
	//[NSApp run];
	while(1){
		//[myview setNeedsDisplay:YES];

		NSEvent *event = [NSApp
			nextEventMatchingMask:NSEventMaskAny
			untilDate:nil		//[NSDate distantPast]
			inMode:NSDefaultRunLoopMode
			dequeue:YES
		];
		if(nil == event)continue;

		switch ([event type]) {
		case NSEventTypeKeyDown:
		case NSEventTypeKeyUp:
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
		case NSEventTypeFlagsChanged:
		default:break;
		}
		[NSApp sendEvent:event];
	}

	free(data);
	return 0;
}
