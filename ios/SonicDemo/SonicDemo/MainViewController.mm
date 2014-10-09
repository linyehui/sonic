//
//  MainViewController.m
//  SonicDemo
//
//  Created by linyehui on 14-2-20.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import "MainViewController.h"
#import "SonicEngine/SonicGenerator.h"

#import <AVFoundation/AVFoundation.h>
#import "WaveListener.h"

@interface MainViewController ()<AVAudioPlayerDelegate>

@property (nonatomic, retain) AVAudioPlayer *audioPlayer;
@property (nonatomic, retain) NSThread *thread;

@end

@implementation MainViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
    
    animationTimer = nil;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    // Listener
    //[WaveListener sharedWaveListener].listenedActionDelegate = self;
    [[WaveListener sharedWaveListener] startListening];
    
    // 20Hz
    animationInterval = 1./20.;
    animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(runComputeWave) userInfo:nil repeats:YES];
    
    NSRunLoop *main = [NSRunLoop currentRunLoop];
    [main addTimer:animationTimer forMode:NSRunLoopCommonModes];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)runComputeWave
{
    [[WaveListener sharedWaveListener] computeWave];
}

- (IBAction)genWaveDataClicked:(id)sender
{
    SonicGenerator *engine = [[SonicGenerator alloc] init];
    
    NSString *code = @"nk104a5dj8";
    NSData *pcmData = [engine genWaveData:code];
    
    NSError *error = nil;
    
    self.audioPlayer = [[AVAudioPlayer alloc] initWithData:pcmData error:&error];
    [self.audioPlayer setVolume:1.0];
    
    if (error)
    {
        NSLog(@"error....%@",[error localizedDescription]);
    }
    else
    {
        self.audioPlayer.delegate = self;
        
        AVAudioSession *audioSession = [AVAudioSession sharedInstance];
        //默认情况下扬声器播放
        [audioSession setCategory:AVAudioSessionCategoryPlayback error:nil];
        [audioSession setActive:YES error:nil];
        [audioSession overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:&error];
        
        [self.audioPlayer prepareToPlay];
    }
    
    [self.audioPlayer play];
}

- (IBAction)computerWaveClicked:(id)sender
{
    [[WaveListener sharedWaveListener] setListening:YES];
}

#pragma mark - AVAudioPlayerDelegate <NSObject>

/* audioPlayerDidFinishPlaying:successfully: is called when a sound has finished playing. This method is NOT called if the player is stopped due to an interruption. */
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
//    if (self.playFlag && [self.audioPlayer play]) {
//        
//        return;
//    }
//    
//    [[AppDelegate sharedAppDelegate] setListenning:YES];
}

/* if an error occurs while decoding it will be reported to the delegate. */
- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error
{
//    [[AppDelegate sharedAppDelegate] setListenning:YES];
}

/* audioPlayerBeginInterruption: is called when the audio session has been interrupted while the player was playing. The player will have been paused. */
- (void)audioPlayerBeginInterruption:(AVAudioPlayer *)player
{
//    [[AppDelegate sharedAppDelegate] setListenning:YES];
}

/* audioPlayerEndInterruption: is called when the preferred method, audioPlayerEndInterruption:withFlags:, is not implemented. */
- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player
{
//    [[AppDelegate sharedAppDelegate] setListenning:YES];
}

@end
