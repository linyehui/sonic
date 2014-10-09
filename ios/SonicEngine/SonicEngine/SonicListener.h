//
//  SonicListener.h
//
//
//  Created by linyehui on 14-2-19.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "CAStreamBasicDescription.h"

@interface SonicListener : NSObject
{
}

- (void)initFFTMgr:(UInt32) inNumberFrames;
- (void)grabAudioData: (AudioBufferList *) inBL;
- (NSString*)computeWave;

@end
