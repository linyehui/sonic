//
//  SonicGenerator.h
//  SonicEngine
//
//  Created by linyehui on 14-3-6.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SonicGenerator : NSObject

// 传入一个10char的字符串，生成8char的rscode之后一起返回
// 返回18char的code+rscode的WAV格式Buffer
- (NSData *) genWaveData: (NSString *)code;

@end
