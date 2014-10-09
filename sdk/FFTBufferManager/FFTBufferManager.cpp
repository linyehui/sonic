/*
 
     File: FFTBufferManager.cpp
 Abstract: This class manages buffering and computation for FFT analysis on input audio data. The methods provided are used to grab the audio, buffer it, and perform the FFT when sufficient data is available
  Version: 1.0
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2011 Apple Inc. All Rights Reserved.
 
 
 */

#include <stdlib.h>
#include "FFTBufferManager.h"
#include "CABitOperations.h"
#include "CAStreamBasicDescription.h"

#include "../iPublicUtility/VectorMath.h"

#define min(x,y) (x < y) ? x : y

FFTBufferManager::FFTBufferManager(unsigned long inNumberFrames) :
mNeedsAudioData(0),
mHasAudioData(0),
mFFTNormFactor(1.0/(2*inNumberFrames)),
mAdjust0DB(1.5849e-13),
m24BitFracScale(16777216.0f),
mFFTLength(inNumberFrames/2),
mLog2N(Log2Ceil(inNumberFrames)),
mNumberFrames(inNumberFrames),
mAudioBufferSize(inNumberFrames * sizeof(float)),
mAudioBufferCurrentIndex(0)

{
    mAudioBuffer = (float*) calloc(mNumberFrames,sizeof(float));
    
    //mDspSplitComplex.realp = (float*) calloc(mFFTLength,sizeof(float));
    //mDspSplitComplex.imagp = (float*) calloc(mFFTLength, sizeof(float));
    mSplitComplexRe = (float*) calloc(mFFTLength,sizeof(float));
    mSplitComplexIm = (float*) calloc(mFFTLength,sizeof(float));
    
    //mSpectrumAnalysis = vDSP_create_fftsetup(mLog2N, kFFTRadix2);
    mKissFFTCfg = kiss_fft_alloc(mFFTLength, 0, NULL, NULL);
	OSAtomicIncrement32Barrier(&mNeedsAudioData);
}

FFTBufferManager::~FFTBufferManager()
{
    //vDSP_destroy_fftsetup(mSpectrumAnalysis);
    kiss_fft_cleanup();
    free(mAudioBuffer);
    //free (mDspSplitComplex.realp);
    //free (mDspSplitComplex.imagp);
    free (mSplitComplexRe);
    free (mSplitComplexIm);
}

void FFTBufferManager::GrabAudioData(AudioBufferList *inBL)
{
	if (mAudioBufferSize < inBL->mBuffers[0].mDataByteSize)	return;
	
	unsigned long bytesToCopy = min(inBL->mBuffers[0].mDataByteSize, mAudioBufferSize - mAudioBufferCurrentIndex);
	memcpy((char*)mAudioBuffer+mAudioBufferCurrentIndex, inBL->mBuffers[0].mData, bytesToCopy);
	
	mAudioBufferCurrentIndex += bytesToCopy / sizeof(float);
	if (mAudioBufferCurrentIndex >= mAudioBufferSize / sizeof(float))
	{
		OSAtomicIncrement32Barrier(&mHasAudioData);
		OSAtomicDecrement32Barrier(&mNeedsAudioData);
	}
}

bool	FFTBufferManager::ComputeFFT(int32_t *outFFTData)
{
	if (HasNewAudioData())
	{
        kiss_fft_stride(mKissFFTCfg, (const kiss_fft_cpx *)mAudioBuffer, (kiss_fft_cpx *)mAudioBuffer, 1);

        //Generate a split complex vector from the real data
        //vDSP_ctoz((COMPLEX *)mAudioBuffer, 2, &mDspSplitComplex, 1, mFFTLength);
        WebCore::VectorMath::ctoz((WebCore::VectorMath::Complex *)mAudioBuffer, 2, mSplitComplexRe, mSplitComplexIm, 1, mFFTLength);
        
        //Take the fft and scale appropriately
        //vDSP_fft_zrip(mSpectrumAnalysis, &mDspSplitComplex, 1, mLog2N, kFFTDirection_Forward);
        
        //vDSP_vsmul(mDspSplitComplex.realp, 1, &mFFTNormFactor, mDspSplitComplex.realp, 1, mFFTLength);
        WebCore::VectorMath::vsmul(mSplitComplexRe, 1, &mFFTNormFactor, mSplitComplexRe, 1, mFFTLength);
        
        //vDSP_vsmul(mDspSplitComplex.imagp, 1, &mFFTNormFactor, mDspSplitComplex.imagp, 1, mFFTLength);
        WebCore::VectorMath::vsmul(mSplitComplexIm, 1, &mFFTNormFactor, mSplitComplexIm, 1, mFFTLength);

        //Zero out the nyquist value
        mSplitComplexIm[0] = 0.0;
        
        //Convert the fft data to dB
        float tmpData[mFFTLength];
        //vDSP_zvmags(&mDspSplitComplex, 1, tmpData, 1, mFFTLength);
        WebCore::VectorMath::zvmags(mSplitComplexRe, mSplitComplexIm, 1, tmpData, 1, mFFTLength);
        
        //In order to avoid taking log10 of zero, an adjusting factor is added in to make the minimum value equal -128dB
        //vDSP_vsadd(tmpData, 1, &mAdjust0DB, tmpData, 1, mFFTLength);
        WebCore::VectorMath::vsadd(tmpData, 1, &mAdjust0DB, tmpData, 1, mFFTLength);
        
        float one = 1;
        //vDSP_vdbcon(tmpData, 1, &one, tmpData, 1, mFFTLength, 0);
        WebCore::VectorMath::vdbcon(tmpData, 1, &one, tmpData, 1, mFFTLength, 0);
        
        //Convert floating point data to integer (Q7.24)
        //vDSP_vsmul(tmpData, 1, &m24BitFracScale, tmpData, 1, mFFTLength);
        WebCore::VectorMath::vsmul(tmpData, 1, &m24BitFracScale, tmpData, 1, mFFTLength);
        
        for(unsigned long i=0; i<mFFTLength; ++i)
            outFFTData[i] = (signed long) tmpData[i];
        
        OSAtomicDecrement32Barrier(&mHasAudioData);
		OSAtomicIncrement32Barrier(&mNeedsAudioData);
		mAudioBufferCurrentIndex = 0;
		return true;
	}
	else if (mNeedsAudioData == 0)
		OSAtomicIncrement32Barrier(&mNeedsAudioData);
	
	return false;
}
