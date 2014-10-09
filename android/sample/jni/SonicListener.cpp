//
//  SonicListener.cpp
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#include <jni.h>
#include "com_duowan_sonic_SonicListener.h"
#include "listener_helper.h"
#include "DCRejectionFilter.h"

#ifdef __cplusplus  //最好有这个，否则被编译器改了函数名字找不到不要怪我
extern "C" {
#endif

DCRejectionFilter g_dcFilter;

/*
 * Class:     com_duowan_sonic_SonicListener
 * Method:    initFFTMgr
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_duowan_sonic_SonicListener_initFFTMgr
  (JNIEnv* env, jclass, jlong inNumberFrames)
{
	SonicSDK::ListenerHelper::instance()->initFFTMgr(inNumberFrames);

	return 0;
}

/*
 * Class:     com_duowan_sonic_SonicListener
 * Method:    releaseFFTMgr
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_duowan_sonic_SonicListener_releaseFFTMgr
  (JNIEnv *, jclass)
{
	SonicSDK::ListenerHelper::instance()->release();
}

/*
 * Class:     com_duowan_sonic_SonicListener
 * Method:    grabAudioData
 * Signature: (JJJ[B)I
 */
JNIEXPORT jint JNICALL Java_com_duowan_sonic_SonicListener_grabAudioData
  (JNIEnv* env,
		  jclass,
		  jlong inNumberFrames,
		  jlong numberChannels,
		  jlong  dataByteSize,
		  jbyteArray buffer)
{
	jboolean isCopy = false;
	jbyteArray pArray = (jbyteArray) env->GetPrimitiveArrayCritical(buffer, &isCopy);

	AudioBufferList ioDataFloat;
	ioDataFloat.mNumberBuffers = 1;

	unsigned short samples_num = dataByteSize / (sizeof(short) * numberChannels);

	ioDataFloat.mBuffers[0].mNumberChannels = numberChannels;
	ioDataFloat.mBuffers[0].mDataByteSize = dataByteSize*2;
	ioDataFloat.mBuffers[0].mData = malloc(ioDataFloat.mBuffers[0].mDataByteSize);

	//memcpy(ioDataFloat.mBuffers[0].mData, pArray, dataByteSize);
	float* tempFloat = (float*)ioDataFloat.mBuffers[0].mData;
	short* tempShort = (short*)pArray;
	for (int j = 0; j < samples_num; j++)
	{
		float f_value = (float) tempShort[j] / 32768.0;
		if( f_value > 1 ) f_value = 1;
		if( f_value < -1 ) f_value = -1;

		tempFloat[j] = f_value;
	}

	// Remove DC component
	g_dcFilter.InplaceFilter((float*)(ioDataFloat.mBuffers[0].mData), samples_num);

	SonicSDK::ListenerHelper::instance()->grabAudioData(&ioDataFloat);

	free(ioDataFloat.mBuffers[0].mData);

	env->ReleasePrimitiveArrayCritical(buffer, pArray, JNI_ABORT);

	return 0;
}

/*
 * Class:     com_duowan_sonic_SonicListener
 * Method:    computeWave
 * Signature: ([B)Z
 */
JNIEXPORT jboolean JNICALL Java_com_duowan_sonic_SonicListener_computeWave
  (JNIEnv *env, jclass, jbyteArray buffer)
{
	jboolean ret = false;
	jboolean isCopy = false;
	jbyteArray pArray = (jbyteArray) env->GetPrimitiveArrayCritical(buffer, &isCopy);

	char result[10] = {0};
	if (SonicSDK::ListenerHelper::instance()->computeWave(result, 10))
	{
		memcpy(pArray, result, 10);
		ret = true;
	}

Exit0:
	env->ReleasePrimitiveArrayCritical(buffer, pArray, 0);
	return ret;
}

#ifdef __cplusplus
}
#endif
