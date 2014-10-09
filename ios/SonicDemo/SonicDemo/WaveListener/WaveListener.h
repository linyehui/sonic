//
//  WaveListener.h
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include <libkern/OSAtomic.h>
#include <CoreFoundation/CFURL.h>

#import "SonicEngine/SonicListener.h"
#import "CAStreamBasicDescription.h"
#import "aurio_helper.h"

#define SPECTRUM_BAR_WIDTH 4

#ifndef CLAMP
#define CLAMP(min,x,max) (x < min ? min : (x > max ? max : x))
#endif


@interface WaveListener : NSObject
{
    SonicListener *listener_engine;
	
	AudioUnit rioUnit;
	BOOL unitIsRunning;
	BOOL unitHasBeenCreated;
			
	BOOL mute;
    BOOL interruption;
	
	DCRejectionFilter* dcFilter;
	CAStreamBasicDescription thruFormat;
    
	Float64 hwSampleRate;
    
    AudioConverterRef audioConverter;
	AURenderCallbackStruct inputProc;
    //SystemSoundID buttonPressSound;
    BOOL _isListening;
}

//@property (nonatomic, retain) EAGLView* view;
@property (nonatomic, retain) SonicListener *listener_engine;

@property (nonatomic, assign) AudioUnit rioUnit;
@property (nonatomic, assign) BOOL unitIsRunning;
@property (nonatomic, assign) BOOL unitHasBeenCreated;
@property (nonatomic, assign) BOOL mute;
@property (nonatomic, assign) AURenderCallbackStruct inputProc;
@property (nonatomic, assign) BOOL interruption;

+ (WaveListener *)sharedWaveListener;

- (void)setListening:(BOOL)state;
- (void)startListening;
- (void)computeWave;

- (void)actionWhenApplicationDidBecomeActive;
- (void)actionWhenApplicationWillResignActive;
- (void)actionWhenApplicationDidEnterBackground;
- (void)actionWhenApplicationWillEnterForeground;

@end
