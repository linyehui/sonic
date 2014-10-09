//
//  listener_helper.cpp
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#include "listener_helper.h"

#include <math.h>
#include "bb_header.h"
#include "queue.h"

#ifdef SONIC_TARGET_OS_ANDROID
#include <android/log.h>
#endif

//////////////////////////////////////////////////////////

#ifndef CLAMP
#define CLAMP(min,x,max) (x < min ? min : (x > max ? max : x))
#endif

SonicSDK::ListenerHelper* SonicSDK::ListenerHelper::ms_instance = 0;

//////////////////////////////////////////////////////////

SonicSDK::ListenerHelper::ListenerHelper()
: fftBufferManager(0)
, l_fftData(0)
, fftData(0)
, fftLength(0)
, hasNewFFTData(false)
, isFreqHigh(false)
{
}

SonicSDK::ListenerHelper::~ListenerHelper()
{
	if (0 != fftBufferManager)
	{
		delete fftBufferManager;
		fftBufferManager = 0;
	}

	if (0 != l_fftData)
	{
		delete l_fftData;
		l_fftData = 0;
	}

	if (0 != fftData)
	{
		delete fftData;
		fftData = 0;
	}
}

SonicSDK::ListenerHelper* SonicSDK::ListenerHelper::instance()
{
	if(0 == ListenerHelper::ms_instance)
	{
		ListenerHelper::ms_instance = new ListenerHelper;
	}
	return ListenerHelper::ms_instance;
}

void SonicSDK::ListenerHelper::release()
{
	if (0 != ListenerHelper::ms_instance)
	{
		delete ListenerHelper::ms_instance;
		ListenerHelper::ms_instance = 0;
	}
}

void SonicSDK::ListenerHelper::initFFTMgr(long inNumberFrames)
{
    fftBufferManager = new FFTBufferManager(inNumberFrames);
    l_fftData = new int32_t[inNumberFrames/2];

    drawFormat.SetAUCanonical(1, false);
    drawFormat.mSampleRate = 44100;

    switch_freq(isFreqHigh);
}

void SonicSDK::ListenerHelper::grabAudioData(AudioBufferList* inBL)
{
    if (fftBufferManager == NULL)
        return;

    if (fftBufferManager->NeedsNewAudioData())
    {
        fftBufferManager->GrabAudioData(inBL);
    }
}

void SonicSDK::ListenerHelper::setFFTData(int32_t* FFTDATA, long LENGTH)
{
	if (LENGTH != fftLength)
	{
		fftLength = LENGTH;
		fftData = (int32_t *)(realloc(fftData, LENGTH * sizeof(int32_t)));
	}
	memmove(fftData, FFTDATA, fftLength * sizeof(float));
	hasNewFFTData = true;
}

bool SonicSDK::ListenerHelper::computeWave(char* code_buffer, long buffer_len)
{
    if (0 == code_buffer || buffer_len < RS_DATA_LEN)
        return false;
    
    if (fftBufferManager->HasNewAudioData())
    {
        //NSLog(@"%@", @"HasNewAudioData\r\n");

        if (fftBufferManager->ComputeFFT(l_fftData))
        {
            setFFTData(l_fftData, fftBufferManager->GetNumberFrames() / 2);

            int i;
            for (i=0; i<32; i++) {

                unsigned int freq;
                int fftIdx;

                num_to_freq(i, &freq);
                fftIdx = freq / (drawFormat.mSampleRate / 2.0) * fftLength;

                double fftIdx_i, fftIdx_f;
                fftIdx_f = modf(fftIdx, &fftIdx_i);

                // char is different in Android and iOS
                // signed for char is must here.
                // linyehui 2014-04-09
                signed char fft_l, fft_r;
                float fft_l_fl, fft_r_fl;
                float interpVal;

                fft_l = (fftData[(int)fftIdx_i] & 0xFF000000) >> 24;
                fft_r = (fftData[(int)fftIdx_i + 1] & 0xFF000000) >> 24;
                fft_l_fl = (float)(fft_l + 80) / 64.;
                fft_r_fl = (float)(fft_r + 80) / 64.;
                interpVal = fft_l_fl * (1. - fftIdx_f) + fft_r_fl * fftIdx_f;

                interpVal = sqrt(CLAMP(0., interpVal, 1.));

                //////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////
                helper(fftIdx, interpVal, 6);///////////
                //////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////

            }

            //////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////
            return helperResultWithTimeSlice(6, code_buffer, buffer_len);///////////////////////////////
            //////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////
        }
        else
        {
            hasNewFFTData = false;
        }
    }
    
    return false;
}

///////////////////////////////////////////////////////////

static queue _savedBuffer[32];

void setupQueue()
{
    static bool flag = false;
    if (!flag)
    {
        for (int i=0; i<32; i++)
        {
            queue q;
            _savedBuffer[i] = q;
            init_queue(&_savedBuffer[i], 20);
        }

        flag = true;
    }
}


void SonicSDK::ListenerHelper::helper(double fftIdx_i, float interpVal, int length)
{
    setupQueue();
    float fff = (drawFormat.mSampleRate / 2.0) * (int)fftIdx_i / (fftLength);

    int code = -1;
    if (freq_to_num(fff, &code) == 0 && code >= 0 && code < 32)
    {
        enqueue(&_savedBuffer[code], interpVal);
        //NSLog(@"%d", code);
    }
}

bool SonicSDK::ListenerHelper::helperResultWithTimeSlice(int length, char* code_buffer, long buffer_len)
{
    if (0 == code_buffer || buffer_len < RS_DATA_LEN)
        return false;
    
    queue *q17 = &_savedBuffer[17];
    queue *q19 = &_savedBuffer[19];

    if (queue_item_at_index(q17, 0) > 0.0 &&
        queue_item_at_index(q17, 1) > 0.0 &&
        queue_item_at_index(q19, 1) > 0.0 &&
        queue_item_at_index(q19, 2) > 0.0) {


        float minValue = fmin(queue_item_at_index(q17, 2), queue_item_at_index(q19, 3));
        minValue = fmax(minValue, queue_item_at_index(q17, 0) * 0.7);

        float maxValue = fmax(queue_item_at_index(q17, 0), queue_item_at_index(q19, 1)) * 1.85;

        int res[20];
        int rrr[20];
        generate_data(_savedBuffer, 32, res, rrr, 20, minValue, maxValue);

        printf("\n================= start:(19[0]=%f), (17[2]=%f), (19[3]=%f), (17[0]*0.7=%f), (minValue=%f) ==================\n\n", queue_item_at_index(q19, 0), queue_item_at_index(q17, 2), queue_item_at_index(q19, 3), queue_item_at_index(q17, 0) * 0.7, minValue);

        for (int i=0; i<20; i++) {
            printf("%02d ", res[i]);
        }

        for (int i=0; i<10; i++) {

            int temp;

            temp = rrr[i];
            rrr[i] = rrr[19-i];
            rrr[19-i] = temp;
        }

        printf("\n");

        for (int i=0; i<20; i++) {
            printf("%02d ", rrr[i]);
        }

        printf("\n\n");


        //////////////  RS

        int temp[18];
        int result[18][18];
        int counter = 0;

        for (int i=0; i<18; i++) {
            for (int j=0; j<18; j++) {

                result[i][j] = -1;
            }
        }

        for (int k = 2; k < 20; k++) {

            printf("~~~~~~~ %02d :17 19 ", k);

            for (int i=2, j=0; i<20; i++, j++) {

                if (i <= k) {
                    temp[j] = res[i];
                }else {
                    temp[j] = rrr[i];
                }

                printf("%02d ", temp[j]);
            }

            printf(" ~~~~~~~ ");

            RS *rs = init_rs(RS_SYMSIZE, RS_GFPOLY, RS_FCR, RS_PRIM, RS_NROOTS, RS_PAD);
            int eras_pos[RS_TOTAL_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

            unsigned char data1[RS_TOTAL_LEN];

            for (int i=0; i<RS_TOTAL_LEN; i++) {
                data1[i] = temp[i];
            }

            int count = decode_rs_char(rs, data1, eras_pos, 0);

            /////////////////

            if (count >= 0) {

                for (int m = 0; m<18; m++) {

                    result[m][counter] = data1[m];
                }

                counter++;
            }

            printf("17 19 ");
            for (int i=0; i<18; i++) {
                printf("%02d ", data1[i]);
            }
            printf("    %d\n", count);
        }

        int temp_vote[18] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int final_result[20] = {17, 19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

        for (int i=0; i<18; i++) {

            for (int j=0; j<18; j++) {

                temp_vote[j] = result[i][j];
            }

            vote(temp_vote, 18, &final_result[i+2]);
        }

        printf("\n ================== final result ================== \n\n");

        if (counter == 0)
        {
            printf("fail!");
#ifdef SONIC_TARGET_OS_ANDROID
            __android_log_print(ANDROID_LOG_WARN, "JNIMsg", "Failed");
#endif
        }
        else
        {
            printf("successes!\n");

            char result_code[RS_DATA_LEN] = {0};

            int result_code_index = 0;
            for (int i=0; i<20; i++) {

                printf("%02d ", final_result[i]);
                //__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "%02d", final_result[i]);

                if (i>=2 && i<=11) {

                    char res_char;
                    num_to_char(final_result[i], &res_char);

                    sprintf(result_code + result_code_index, "%c", res_char);
                    ++result_code_index;
                }
            }
            memcpy(code_buffer, result_code, RS_DATA_LEN);
            
#ifdef SONIC_TARGET_OS_ANDROID
            __android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Successes: %s", (char*)result_code);
#endif
        }

        printf("\n\n================  end  ==================\n");

        //printf("\n");
        
        return (counter > 0);
    }
    
    return false;
}
