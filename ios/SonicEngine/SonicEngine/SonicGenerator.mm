//
//  SonicGenerator.m
//  SonicEngine
//
//  Created by linyehui on 14-3-6.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import "SonicGenerator.h"

#import "bb_header.h"
#import "queue.h"
#import "pcm_render.h"
#import "generator_helper.h"

@implementation SonicGenerator

- (NSData *) genWaveData: (NSString *)code
{
    if (!code || ![code isKindOfClass:[NSString class]] || [code length] != RS_DATA_LEN)
        return nil;
        
    const char *src = [code UTF8String];
    char result_with_rs[RS_TOTAL_LEN + 1];
    if (!SonicSDK::GeneratorHelper::genRSCode(src, result_with_rs, RS_TOTAL_LEN + 1))
        return nil;
    
    long buffer_len_by_byte = SonicSDK::PCMRender::getChirpLengthByByte(RS_TOTAL_LEN);
    short* wave_buffer = (short*)malloc(buffer_len_by_byte);
    if (NULL == wave_buffer)
        return nil;

    unsigned char waveHeaderByteArray[SonicSDK::SONIC_HEADER_SIZE];
    if (!SonicSDK::PCMRender::renderChirpData(
                                            result_with_rs,
                                            RS_TOTAL_LEN,
                                            waveHeaderByteArray,
                                            SonicSDK::SONIC_HEADER_SIZE,
                                            wave_buffer,
                                            buffer_len_by_byte))
    {
        free(wave_buffer);
        return nil;
    }
    

    NSMutableData *chirpData = [[NSMutableData alloc] initWithBytes:waveHeaderByteArray length:sizeof(waveHeaderByteArray)];
    [chirpData appendBytes:wave_buffer length:(buffer_len_by_byte)];
    
    free(wave_buffer);
    
    return chirpData;
}


@end
