//
//  generator_helper.h
//
//
//  Created by linyehui on 2014-02-20.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#ifndef __GENERATOR_HELPER_H__
#define __GENERATOR_HELPER_H__

namespace SonicSDK
{
    class GeneratorHelper
    {
    public:
        static bool genRSCode(const char *src, char* result_with_rs, unsigned int result_len);
    };
    
} // namespace SonicSDK

#endif // __GENERATOR_HELPER_H__
