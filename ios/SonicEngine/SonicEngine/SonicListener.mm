//
//  SonicListener.mm
//
//
//  Created by linyehui on 14-2-19.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import "SonicListener.h"
#import "listener_helper.h"

@implementation SonicListener

-(void)dealloc
{
    SonicSDK::ListenerHelper::instance()->release();
    [super dealloc];
}

- (void)initFFTMgr:(UInt32) inNumberFrames
{
    SonicSDK::ListenerHelper::instance()->initFFTMgr(inNumberFrames);
}

- (void)grabAudioData: (AudioBufferList *) inBL
{
    SonicSDK::ListenerHelper::instance()->grabAudioData(inBL);
}

- (NSString*)computeWave
{
    char code[10] = {0};
    if (SonicSDK::ListenerHelper::instance()->computeWave(code, 10))
    {
        NSMutableString *string_code = [NSMutableString stringWithFormat:@""];
        for (int i = 0; i < 10; i++)
        {
            [string_code appendFormat:@"%c", code[i]];
        }
        
        return string_code;
    }
    else
    {
        return nil;
    }
}

@end
