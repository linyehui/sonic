//
//  pcm_render.h
//
//
//  Created by linyehui on 2014-03-06.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#ifndef __PCM_RENDER_H__
#define __PCM_RENDER_H__

namespace SonicSDK
{
    const unsigned int SONIC_HEADER_SIZE = 44;

    class PCMRender
    {
    public:
        static int getChirpLengthByByte(unsigned int data_len);
        
        static bool renderChirpData(
                                    const char* code_with_rs,
                                    unsigned int data_len,
                                    unsigned char* header,
                                    unsigned int header_len,
                                    short* wave_buffer,
                                    long buffer_len_by_byte);
    };
    
}

#endif // SonicSDK

