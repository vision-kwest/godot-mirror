/*************************************************************************/
/*  app_delegate.mm                                                      */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#import "app_delegate.h"
#import "gl_view.h"

#include "os_iphone.h"
#include "core/globals.h"
#include "main/main.h"

#ifdef MODULE_FACEBOOKSCORER_IOS_ENABLED
#include "modules/FacebookScorer_ios/FacebookScorer.h"
#endif

#ifdef MODULE_GAME_ANALYTICS_ENABLED
#import "modules/game_analytics/ios/MobileAppTracker.framework/Headers/MobileAppTracker.h"
//#import "modules/game_analytics/ios/MobileAppTracker.h"
#import <AdSupport/AdSupport.h>
#endif

#ifdef MODULE_PARSE_ENABLED
#import <Parse/Parse.h>
#import "FBSDKCoreKit/FBSDKCoreKit.h"
#endif

#define kFilteringFactor                        0.1
#define kRenderingFrequency						60
#define kAccelerometerFrequency         100.0 // Hz

#ifdef APPIRATER_ENABLED
#import "Appirater.h"
#endif

Error _shell_open(String p_uri) {
	NSString* url = [[NSString alloc] initWithUTF8String:p_uri.utf8().get_data()];

    if (![[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:url]])
        return ERR_CANT_OPEN;

    printf("opening url %ls\n", p_uri.c_str());
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
	[url release];
	return OK;
};

@implementation AppDelegate

@synthesize window;

extern int gargc;
extern char** gargv;
extern int iphone_main(int, int, int, char**);
extern void iphone_finish();

static ViewController* mainViewController = nil;
+ (ViewController*) getViewController
{
	return mainViewController;
}

static int frame_count = 0;
- (void)drawView:(GLView*)view; {

	switch (frame_count) {

	case 0: {
        int backingWidth;
        int backingHeight;
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
		

		OS::VideoMode vm;
		vm.fullscreen = true;
		vm.width = backingWidth;
		vm.height = backingHeight;
		vm.resizable = false;
		OS::get_singleton()->set_video_mode(vm);

		if (!OS::get_singleton()) {
			exit(0);
		};
		++frame_count;

		NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		//NSString *documentsDirectory = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
		OSIPhone::get_singleton()->set_data_dir(String::utf8([documentsDirectory UTF8String]));
		
		NSString *locale_code = [[[NSLocale preferredLanguages] objectAtIndex:0] substringToIndex:2];
		OSIPhone::get_singleton()->set_locale(String::utf8([locale_code UTF8String]));

		NSString* uuid;
		if ([[UIDevice currentDevice]respondsToSelector:@selector(identifierForVendor)]) {
			uuid = [UIDevice currentDevice].identifierForVendor.UUIDString;
		}else{

			// before iOS 6, so just generate an identifier and store it
			uuid = [[NSUserDefaults standardUserDefaults] objectForKey:@"identiferForVendor"];
			if( !uuid ) {
				CFUUIDRef cfuuid = CFUUIDCreate(NULL);
				uuid = (__bridge_transfer NSString*)CFUUIDCreateString(NULL, cfuuid);
				CFRelease(cfuuid);
				[[NSUserDefaults standardUserDefaults] setObject:uuid forKey:@"identifierForVendor"];
			}
		}

		OSIPhone::get_singleton()->set_unique_ID(String::utf8([uuid UTF8String]));

	}; break;
/*
	case 1: {
		++frame_count;
	} break;
*/
	case 1: {

		Main::setup2();
		++frame_count;

		// this might be necessary before here
		NSDictionary* dict = [[NSBundle mainBundle] infoDictionary];
		for (NSString* key in dict) {
			NSObject* value = [dict objectForKey:key];
			String ukey = String::utf8([key UTF8String]);

			// we need a NSObject to Variant conversor

			if ([value isKindOfClass:[NSString class]]) {
				NSString* str = (NSString*)value;
				String uval = String::utf8([str UTF8String]);

				Globals::get_singleton()->set("Info.plist/"+ukey, uval);

			} else if ([value isKindOfClass:[NSNumber class]]) {

				NSNumber* n = (NSNumber*)value;
				double dval = [n doubleValue];

				Globals::get_singleton()->set("Info.plist/"+ukey, dval);
			};
			// do stuff
		}

	} break;
/*
	case 3: {
		++frame_count;
	} break;
*/
	case 2: {

		Main::start();
		++frame_count;

		#ifdef APPIRATER_ENABLED
		int aid = Globals::get_singleton()->get("ios/app_id");
		[Appirater appLaunched:YES app_id:aid];
		#endif

	}; break; // no fallthrough

	default: {

		if (OSIPhone::get_singleton()) {
			OSIPhone::get_singleton()->update_accelerometer(accel[0], accel[1], accel[2]);
			bool quit_request = OSIPhone::get_singleton()->iterate();
		};

	};

	};
};

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {

	printf("****************** did receive memory warning!\n");
	OS::get_singleton()->get_main_loop()->notification(MainLoop::NOTIFICATION_OS_MEMORY_WARNING);
};

- (void)applicationDidFinishLaunching:(UIApplication*)application {

	printf("**************** app delegate init\n");
	CGRect rect = [[UIScreen mainScreen] bounds];

	[application setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	// disable idle timer
	application.idleTimerDisabled = YES;

	//Create a full-screen window
	window = [[UIWindow alloc] initWithFrame:rect];
	//window.autoresizesSubviews = YES;
	//[window setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleWidth];

	//Create the OpenGL ES view and add it to the window
	GLView *glView = [[GLView alloc] initWithFrame:rect];
	printf("glview is %p\n", glView);
	//[window addSubview:glView];
	glView.delegate = self;
	//glView.autoresizesSubviews = YES;
	//[glView setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleWidth];

    int backingWidth;
    int backingHeight;
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    iphone_main(backingWidth, backingHeight, gargc, gargv);
    
	view_controller = [[ViewController alloc] init];
	view_controller.view = glView;
	window.rootViewController = view_controller;

	glView.animationInterval = 1.0 / kRenderingFrequency;
	[glView startAnimation];
	
	//Show the window
	[window makeKeyAndVisible];

	//Configure and start accelerometer
	last_accel[0] = 0;
	last_accel[1] = 0;
	last_accel[2] = 0;
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];

	//OSIPhone::screen_width = rect.size.width - rect.origin.x;
	//OSIPhone::screen_height = rect.size.height - rect.origin.y;
	
	mainViewController = view_controller;
    
#ifdef MODULE_GAME_ANALYTICS_ENABLED
    printf("********************* didFinishLaunchingWithOptions\n");
    if(!Globals::get_singleton()->has("mobileapptracker/advertiser_id"))
    {
        return;
    }
    if(!Globals::get_singleton()->has("mobileapptracker/conversion_key"))
    {
        return;
    }
        
    String adid = GLOBAL_DEF("mobileapptracker/advertiser_id","");
    String convkey = GLOBAL_DEF("mobileapptracker/conversion_key","");
        
    NSString * advertiser_id = [NSString stringWithUTF8String:adid.utf8().get_data()];
    NSString * conversion_key = [NSString stringWithUTF8String:convkey.utf8().get_data()];
        
    // Account Configuration info - must be set
    [MobileAppTracker initializeWithMATAdvertiserId:advertiser_id
                                    MATConversionKey:conversion_key];
        
    // Used to pass us the IFA, enables highly accurate 1-to-1 attribution.
    // Required for many advertising networks.
    [MobileAppTracker setAppleAdvertisingIdentifier:[[ASIdentifierManager sharedManager] advertisingIdentifier]
        advertisingTrackingEnabled:[[ASIdentifierManager sharedManager] isAdvertisingTrackingEnabled]];
        
#endif
    
};

- (void)applicationWillTerminate:(UIApplication*)application {

	printf("********************* will terminate\n");
	iphone_finish();
};

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	printf("********************* did enter background\n");
	if (OS::get_singleton()->get_main_loop())
		OS::get_singleton()->get_main_loop()->notification(MainLoop::NOTIFICATION_WM_FOCUS_OUT);
	[view_controller.view stopAnimation];
	if (OS::get_singleton()->native_video_is_playing()) {
		OSIPhone::get_singleton()->native_video_focus_out();
	};
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	printf("********************* did enter foreground\n");
	//OS::get_singleton()->get_main_loop()->notification(MainLoop::NOTIFICATION_WM_FOCUS_IN);
	[view_controller.view startAnimation];
}

- (void) applicationWillResignActive:(UIApplication *)application
{
	printf("********************* will resign active\n");
	//OS::get_singleton()->get_main_loop()->notification(MainLoop::NOTIFICATION_WM_FOCUS_OUT);
	[view_controller.view stopAnimation]; // FIXME: pause seems to be recommended elsewhere
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
	printf("********************* did become active\n");
#ifdef MODULE_GAME_ANALYTICS_ENABLED
    printf("********************* mobile app tracker found\n");
	[MobileAppTracker measureSession];
#endif
	if (OS::get_singleton()->get_main_loop())
		OS::get_singleton()->get_main_loop()->notification(MainLoop::NOTIFICATION_WM_FOCUS_IN);
	[view_controller.view startAnimation]; // FIXME: resume seems to be recommended elsewhere
	if (OSIPhone::get_singleton()->native_video_is_playing()) {
		OSIPhone::get_singleton()->native_video_unpause();
	};
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration {
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
	accel[0] = acceleration.x; // * kFilteringFactor + accel[0] * (1.0 - kFilteringFactor);
	accel[1] = acceleration.y; // * kFilteringFactor + accel[1] * (1.0 - kFilteringFactor);
	accel[2] = acceleration.z; // * kFilteringFactor + accel[2] * (1.0 - kFilteringFactor);
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url {
#ifdef MODULE_FACEBOOKSCORER_IOS_ENABLED
	return [[[FacebookScorer sharedInstance] facebook] handleOpenURL:url];
#else
	return false;
#endif
}

// For 4.2+ support
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
#ifdef MODULE_PARSE_ENABLED
	NSLog(@"Handling application openURL");
	return [[FBSDKApplicationDelegate sharedInstance] application:application
														  openURL:url
												sourceApplication:sourceApplication
													   annotation:annotation];
#endif


#ifdef MODULE_FACEBOOKSCORER_IOS_ENABLED
	return [[[FacebookScorer sharedInstance] facebook] handleOpenURL:url];
#else
	return false;
#endif
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
#ifdef MODULE_PARSE_ENABLED
	// Store the deviceToken in the current installation and save it to Parse.
	PFInstallation *currentInstallation = [PFInstallation currentInstallation];
	//NSString* token = [[NSString alloc] initWithData:deviceToken encoding:NSUTF8StringEncoding];
	NSLog(@"Device Token : %@ ", deviceToken);
	[currentInstallation setDeviceTokenFromData:deviceToken];
	[currentInstallation saveInBackground];
#endif
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
#ifdef MODULE_PARSE_ENABLED
	[PFPush handlePush:userInfo];
	NSDictionary *aps = [userInfo objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	NSLog(@"Push Notification Payload (app active) %@", aps);
	[defaults setObject:aps forKey:@"notificationInfo"];
	[defaults synchronize];
	if (application.applicationState == UIApplicationStateInactive) {
		[PFAnalytics trackAppOpenedWithRemoteNotificationPayload:userInfo];
	}
#endif
}

- (void)dealloc
{
	[window release];
	[super dealloc];
}

@end
