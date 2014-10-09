//
//  listener_helper.h
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#ifndef __LISTENER_HELPER_H__
#define __LISTENER_HELPER_H__

#include "FFTBufferManager.h"
#include "CAStreamBasicDescription.h"

//////////////////////////////////////////////////////////////////

namespace SonicSDK
{
    class ListenerHelper
    {
    protected:
        ListenerHelper();
        virtual ~ListenerHelper();
    public:
        static ListenerHelper* instance();
        static void release();
        
        void initFFTMgr(long inNumberFrames);
        void grabAudioData(AudioBufferList* inBL);
        void setFFTData(int32_t* FFTDATA, long LENGTH);
        bool computeWave(char* code_buffer, long buffer_len);
        
    protected:
        void helper(double fftIdx_i, float interpVal, int length);
        bool helperResultWithTimeSlice(int length, char* code_buffer, long buffer_len);
        
    private:
        static ListenerHelper* 	ms_instance;
        
        FFTBufferManager* 			fftBufferManager;
        int32_t* 					l_fftData;
        
        int32_t* 					fftData;
        long 						fftLength;
        bool 						hasNewFFTData;
        
        CAStreamBasicDescription 	drawFormat;
        bool 						isFreqHigh;
        
    };
    
} // namespace SonicSDK

#endif // __LISTENER_HELPER_H__
