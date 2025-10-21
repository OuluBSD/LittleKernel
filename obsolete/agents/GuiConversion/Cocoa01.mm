#ifdef HAVE_COCOA
#import <Cocoa/Cocoa.h>

@interface HelloAppDelegate : NSObject <NSApplicationDelegate>
-(IBAction)onButtonClick:(id)sender;
@end

@implementation HelloAppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}
-(IBAction)onButtonClick:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Popup message"];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}
@end

void Cocoa01(int test_num) {
    if (test_num == 0 || test_num < 0) {
        @autoreleasepool {
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

            NSRect frame = NSMakeRect(100, 100, 320, 240);
            NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                            styleMask:(NSWindowStyleMaskTitled |
                                                                       NSWindowStyleMaskClosable |
                                                                       NSWindowStyleMaskMiniaturizable)
                                                              backing:NSBackingStoreBuffered
                                                                defer:NO];
            [window setTitle:@"Hello World program"];

            NSTextField *label = [[NSTextField alloc] initWithFrame:[[window contentView] bounds]];
            [label setStringValue:@"Hello world!"];
            [label setBezeled:NO];
            [label setDrawsBackground:NO];
            [label setEditable:NO];
            [label setSelectable:NO];
            [label setAlignment:NSTextAlignmentCenter];
            [label setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
            [[window contentView] addSubview:label];

            [window center];
            [window makeKeyAndOrderFront:nil];

            HelloAppDelegate *delegate = [[HelloAppDelegate alloc] init];
            [NSApp setDelegate:delegate];
            [NSApp activateIgnoringOtherApps:YES];

            [NSApp run];

            // keep objects alive until after the run loop ends
            (void)delegate; (void)window; (void)label;
        }
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        @autoreleasepool {
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

            NSRect frame = NSMakeRect(100, 100, 320, 240);
            NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                            styleMask:(NSWindowStyleMaskTitled |
                                                                       NSWindowStyleMaskClosable |
                                                                       NSWindowStyleMaskMiniaturizable)
                                                              backing:NSBackingStoreBuffered
                                                                defer:NO];
            [window setTitle:@"Button program"];

            NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(30, 30, 100, 30)];
            [button setTitle:@"Hello world!"];
            [button setBezelStyle:NSBezelStyleRounded];
            HelloAppDelegate *delegate = [[HelloAppDelegate alloc] init];
            [button setTarget:delegate];
            [button setAction:@selector(onButtonClick:)];
            [[window contentView] addSubview:button];

            [window center];
            [window makeKeyAndOrderFront:nil];

            [NSApp setDelegate:delegate];
            [NSApp activateIgnoringOtherApps:YES];

            [NSApp run];

            // keep objects alive until after the run loop ends
            (void)delegate; (void)window; (void)button;
        }
    }
}
#else
void Cocoa01(int) {}
#endif
