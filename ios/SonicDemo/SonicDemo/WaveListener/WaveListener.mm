//
//  WaveListener.cpp
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import "WaveListener.h"

#import "AudioUnit/AudioUnit.h"
#import "CAXException.h"
#import <AVFoundation/AVFoundation.h>
#import "SonicEngine/SonicGenerator.h"
#import <UIKit/UIKit.h>

@implementation WaveListener

// value, a, r, g, b
GLfloat colorLevels[] = {
    0., 1., 0., 0., 0.,
    .333, 1., .7, 0., 0.,
    .667, 1., 0., 0., 1.,
    1., 1., 0., 1., 1.,
};

//@synthesize view;
@synthesize listener_engine;

@synthesize rioUnit;
@synthesize unitIsRunning;
@synthesize unitHasBeenCreated;
@synthesize mute;
@synthesize inputProc;
@synthesize interruption;

#pragma mark -Audio Session Interruption Listener

void rioInterruptionListener(void *inClientData, UInt32 inInterruption)
{
    try {
        printf("Session interrupted! --- %s ---", inInterruption == kAudioSessionBeginInterruption ? "Begin Interruption" : "End Interruption");
        
        WaveListener *THIS = (__bridge WaveListener *)inClientData;
        
        if (inInterruption == kAudioSessionEndInterruption) {
            // make sure we are again the active session
            XThrowIfError(AudioSessionSetActive(true), "couldn't set audio session active");
            XThrowIfError(AudioOutputUnitStart(THIS->rioUnit), "couldn't start unit");
            
            THIS->interruption = NO;
        }
        
        if (inInterruption == kAudioSessionBeginInterruption) {
            
            THIS->interruption = YES;
            
            XThrowIfError(AudioOutputUnitStop(THIS->rioUnit), "couldn't stop unit");
        }
    } catch (CAXException e) {
        char buf[256];
        fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
    }
}

#pragma mark -Audio Session Property Listener

void propListener(	void *                  inClientData,
                  AudioSessionPropertyID	inID,
                  UInt32                  inDataSize,
                  const void *            inData)
{
	WaveListener *THIS = (__bridge WaveListener *)inClientData;
	if (inID == kAudioSessionProperty_AudioRouteChange)
	{
		try {
            UInt32 isAudioInputAvailable;
            UInt32 size = sizeof(isAudioInputAvailable);
            XThrowIfError(AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &size, &isAudioInputAvailable), "couldn't get AudioSession AudioInputAvailable property value");
            
            if(THIS->unitIsRunning && !isAudioInputAvailable)
            {
                XThrowIfError(AudioOutputUnitStop(THIS->rioUnit), "couldn't stop unit");
                THIS->unitIsRunning = false;
            }
            
            else if(!THIS->unitIsRunning && isAudioInputAvailable)
            {
                XThrowIfError(AudioSessionSetActive(true), "couldn't set audio session active\n");
                
                if (!THIS->unitHasBeenCreated)	// the rio unit is being created for the first time
                {
                    XThrowIfError(SetupRemoteIO(THIS->rioUnit, THIS->inputProc, THIS->thruFormat), "couldn't setup remote i/o unit");
                    THIS->unitHasBeenCreated = true;
                    
                    THIS->dcFilter = new DCRejectionFilter[THIS->thruFormat.NumberChannels()];
                    
                    UInt32 maxFPS;
                    size = sizeof(maxFPS);
                    XThrowIfError(AudioUnitGetProperty(THIS->rioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &maxFPS, &size), "couldn't get the remote I/O unit's max frames per slice");
                    
                    [THIS->listener_engine initFFTMgr:maxFPS];
                }
                
                XThrowIfError(AudioOutputUnitStart(THIS->rioUnit), "couldn't start unit");
                THIS->unitIsRunning = true;
            }
            
			// we need to rescale the sonogram view's color thresholds for different input
			CFStringRef newRoute;
			size = sizeof(CFStringRef);
			XThrowIfError(AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &newRoute), "couldn't get new audio route");
			if (newRoute)
			{
				CFShow(newRoute);
				if (CFStringCompare(newRoute, CFSTR("Headset"), NULL) == kCFCompareEqualTo) // headset plugged in
				{
					colorLevels[0] = .3;
					colorLevels[5] = .5;
				}
				else if (CFStringCompare(newRoute, CFSTR("Receiver"), NULL) == kCFCompareEqualTo) // headset plugged in
				{
					colorLevels[0] = 0;
					colorLevels[5] = .333;
					colorLevels[10] = .667;
					colorLevels[15] = 1.0;
					
				}
				else
				{
					colorLevels[0] = 0;
					colorLevels[5] = .333;
					colorLevels[10] = .667;
					colorLevels[15] = 1.0;
					
				}
			}
		} catch (CAXException e) {
			char buf[256];
			fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
		}
		
	}
}

#pragma mark -RIO Render Callback

static OSStatus	PerformThru(
							void						*inRefCon,
							AudioUnitRenderActionFlags 	*ioActionFlags,
							const AudioTimeStamp 		*inTimeStamp,
							UInt32 						inBusNumber,
							UInt32 						inNumberFrames,
							AudioBufferList 			*ioData)
{
	WaveListener *THIS = (__bridge WaveListener *)inRefCon;
	OSStatus err = AudioUnitRender(THIS->rioUnit, ioActionFlags, inTimeStamp, 1, inNumberFrames, ioData);
	if (err)
    {
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
        NSLog(@"Error: %@", [error description]);

        return err;
    }
    
    AudioBufferList ioDataFloat;
    ioDataFloat.mNumberBuffers = ioData->mNumberBuffers;
    for (int i = 0; i < ioDataFloat.mNumberBuffers; i++)
    {
        UInt16 numSamples = inNumberFrames * ioData->mBuffers[i].mNumberChannels;
        
        ioDataFloat.mBuffers[i].mNumberChannels = ioData->mBuffers[i].mNumberChannels;
        ioDataFloat.mBuffers[i].mDataByteSize = numSamples * sizeof(float);
        ioDataFloat.mBuffers[i].mData = malloc(ioDataFloat.mBuffers[i].mDataByteSize);
        
        float* tempFloat = (float*)ioDataFloat.mBuffers[i].mData;
        short* tempShort = (short*)ioData->mBuffers[i].mData;
        for (int j = 0; j < numSamples; j++)
        {
            float f_value = ((float) tempShort[j]) / (float) 32768;
            if( f_value > 1 ) f_value = 1;
            if( f_value < -1 ) f_value = -1;
            
            tempFloat[j] = f_value;
        }
    }
    
    // Remove DC component
    UInt16 samples_num = inNumberFrames*ioDataFloat.mBuffers[0].mNumberChannels;
	for(UInt32 i = 0; i < ioDataFloat.mNumberBuffers; ++i)
		THIS->dcFilter[i].InplaceFilter((Float32*)(ioDataFloat.mBuffers[i].mData), samples_num);
    
    [THIS->listener_engine grabAudioData:&ioDataFloat];

    for (int i = 0; i < ioDataFloat.mNumberBuffers; i++)
    {
        free(ioDataFloat.mBuffers[i].mData);
    }
    
    if (THIS->mute == YES)
    {
        SilenceData(ioData);
    }
	
	return err;
}

#pragma mark-

- (void)setupListening
{
    CFURLRef url = NULL;
	try {
		__weak WaveListener *weakSelf = self;
		void *inClientData = (__bridge void*)weakSelf;

		// Initialize and configure the audio session
		OSStatus status = AudioSessionInitialize(NULL, NULL, rioInterruptionListener, inClientData);
		if (status != kAudioSessionNoError && status != kAudioSessionAlreadyInitialized) {
			NSLog(@"setupListening failed, %d", (int)status);
			return;
		}

        self.interruption = NO;
		UInt32 audioCategory = kAudioSessionCategory_PlayAndRecord;
		XThrowIfError(AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory), "couldn't set audio category");
		XThrowIfError(AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, propListener, inClientData), "couldn't set property listener");
        
		//Float32 preferredBufferSize = .0872;
        Float32 preferredBufferSize = .0873;
        
		XThrowIfError(AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, sizeof(preferredBufferSize), &preferredBufferSize), "couldn't set i/o buffer duration");
		
		UInt32 size = sizeof(hwSampleRate);
		XThrowIfError(AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, &size, &hwSampleRate), "couldn't get hw sample rate");
		
		XThrowIfError(AudioSessionSetActive(true), "couldn't set audio session active\n");
        
		XThrowIfError(SetupRemoteIO(rioUnit, inputProc, thruFormat), "couldn't setup remote i/o unit");
		unitHasBeenCreated = true;
        		
		dcFilter = new DCRejectionFilter[thruFormat.NumberChannels()];
        
		UInt32 maxFPS;
		size = sizeof(maxFPS);
		XThrowIfError(AudioUnitGetProperty(rioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &maxFPS, &size), "couldn't get the remote I/O unit's max frames per slice");
		
        listener_engine = [[SonicListener alloc] init];
        [listener_engine initFFTMgr:maxFPS];
		
		XThrowIfError(AudioOutputUnitStart(rioUnit), "couldn't start remote i/o unit");
        
		size = sizeof(thruFormat);
		XThrowIfError(AudioUnitGetProperty(rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &thruFormat, &size), "couldn't get the remote I/O unit's output client format");
		
		unitIsRunning = 1;
	}
	catch (CAXException &e) {
		char buf[256];
		fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
		unitIsRunning = 0;
		if (dcFilter) delete[] dcFilter;
		if (url) CFRelease(url);
	}
	catch (...) {
		fprintf(stderr, "An unknown error occurred\n");
		unitIsRunning = 0;
		if (dcFilter) delete[] dcFilter;
		if (url) CFRelease(url);
	}
}


- (void)startListening
{
    //_isListening = YES;

	if ([[AVAudioSession sharedInstance] respondsToSelector:@selector(requestRecordPermission:)]) {
		[[AVAudioSession sharedInstance] performSelector:@selector(requestRecordPermission:) withObject:^(BOOL granted) {
			if (granted) {
				// Microphone enabled code
				NSLog(@"Microphone is enabled..");
			} else {
				// Microphone disabled code
				NSLog(@"Microphone is disabled..");
				UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"没有开启麦克风"
																	message:@"请到[设置]->[隐私]->[麦克风]中开启"
																   delegate:nil
														  cancelButtonTitle:@"确定"
														  otherButtonTitles:nil];
				[alertView show];
			}
		}];
	}
	
	// mute should be on at launch
	self.mute = YES;

	// Initialize our remote i/o unit
	inputProc.inputProc = PerformThru;

	__weak WaveListener *weakSelf = self;
	inputProc.inputProcRefCon = (__bridge void*)weakSelf;

    [self setupListening];
}

- (void)actionWhenApplicationDidBecomeActive {
    
	//start animation now that we're in the foreground
//    view.applicationResignedActive = NO;
//	[view startAnimation];
	AudioSessionSetActive(true);
    AudioOutputUnitStart(self.rioUnit);
}

- (void)actionWhenApplicationWillResignActive {
	//stop animation before going into background
//    view.applicationResignedActive = YES;
//    [view stopAnimation];
}

- (void)actionWhenApplicationDidEnterBackground {
}

- (void)actionWhenApplicationWillEnterForeground {
}

- (void)dealloc
{
	delete[] dcFilter;
}

- (void)computeWave
{
    if (_isListening == YES)
    {
        NSString* resultCode = [listener_engine computeWave];
        if (resultCode.length > 0)
        {
            [[WaveListener sharedWaveListener] setListening:NO];
            
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:resultCode message:nil delegate:nil cancelButtonTitle:@"cancel" otherButtonTitles:nil, nil];
            [alertView show];
        }
    }
}

static WaveListener *kSharedObject = nil;

+ (WaveListener *)sharedWaveListener {
    
    @synchronized (self)
    {
        if (kSharedObject == nil)
        {
            kSharedObject = [[self alloc] init];
        }
    }
    return kSharedObject;
}

- (void)setListening:(BOOL)state {
    
    _isListening = state;
}

@end
