//
//  generator_helper.cpp
//
//
//  Created by linyehui on 2014-02-20.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#include "generator_helper.h"
#include "rscode.h"
#include "bb_freq_util.h"

namespace SonicSDK
{
    bool GeneratorHelper::genRSCode(const char *src, char* result_with_rs, unsigned int result_len)
    {
        if (NULL == src
            || NULL == result_with_rs
            || result_len != (RS_TOTAL_LEN + 1))
            return false;
        if (strlen(src) != RS_DATA_LEN)
            return false;
        
        unsigned char data[RS_TOTAL_LEN];
        
        for (int i=0; i<RS_TOTAL_LEN; i++) {
            
            if (i < RS_DATA_LEN) {
                
                char_to_num(src[i], (unsigned int *)(data+i));
                
            } else {
                
                data[i] = 0;
            }
        }
        
        unsigned char *code = data + RS_DATA_LEN;
        
        RS *rs = init_rs(RS_SYMSIZE, RS_GFPOLY, RS_FCR, RS_PRIM, RS_NROOTS, RS_PAD);
        encode_rs_char(rs, data, code);
        
        for (int i=0; i<RS_TOTAL_LEN; i++) {
            
            num_to_char(data[i], result_with_rs + i);
        }
        
        result_with_rs[RS_TOTAL_LEN] = '\0';
        
        return true;
    }
    
} // namespace SonicSDK
