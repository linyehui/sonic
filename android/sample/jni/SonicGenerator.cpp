//
//  SonicGenerator.cpp
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//
#include <jni.h>
#include "com_duowan_sonic_SonicGenerator.h"

#include "freq_util/bb_header.h"
#include "queue/queue.h"
#include "generator_helper/generator_helper.h"
#include "pcm_render/pcm_render.h"

#ifdef __cplusplus  //最好有这个，否则被编译器改了函数名字找不到不要怪我
extern "C" {
#endif

int getWaveLenByByte()
{
	int len = SonicSDK::PCMRender::getChirpLengthByByte(RS_TOTAL_LEN) + SonicSDK::SONIC_HEADER_SIZE;
	return len;
}

int genWaveData(const char* hash_array, char* buffer, int buffer_len)
{
	if (0 == hash_array || 0 == buffer)
			return -1;
	    int hash_len = strlen(hash_array);
	    if (hash_len != RS_DATA_LEN)
	    	return -1;

	    if (buffer_len < getWaveLenByByte())
	    	return -1;

	    char result_with_rs[RS_TOTAL_LEN + 1];
	    if (!SonicSDK::GeneratorHelper::genRSCode(hash_array, result_with_rs, RS_TOTAL_LEN + 1))
	    {
	    	return 0;
	    }

	    long buffer_len_by_byte = SonicSDK::PCMRender::getChirpLengthByByte(RS_TOTAL_LEN);
	    short* wave_buffer = (short*)malloc(buffer_len_by_byte);
	    if (NULL == wave_buffer)
	    	return 0;

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
	        return 0;
	    }

		memcpy(buffer, waveHeaderByteArray, SonicSDK::SONIC_HEADER_SIZE);
		memcpy(buffer + SonicSDK::SONIC_HEADER_SIZE, wave_buffer, buffer_len_by_byte);
		//memcpy(pArray, wave_buffer, buffer_len_by_byte);

	    if (0 != wave_buffer)
	    {
	    	free(wave_buffer);
	    }

	    jint output_len = SonicSDK::SONIC_HEADER_SIZE + buffer_len_by_byte;
	    return output_len;
}

/*
 * Class:     com_duowan_sonic_SonicGenerator
 * Method:    getWaveLenByByte
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_duowan_sonic_SonicGenerator_getWaveLenByByte
  (JNIEnv *, jclass)
{
	return getWaveLenByByte();
}

/*
 * Class:     com_duowan_sonic_SonicGenerator
 * Method:    genWaveData
 * Signature: (Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_com_duowan_sonic_SonicGenerator_genWaveData
  (JNIEnv* env, jclass, jstring hash_string, jbyteArray buffer)
{
	const char* pHashString = (const char*)env->GetStringUTFChars(hash_string, JNI_FALSE);
	if (0 == pHashString)
		return -1;

	// env的调用好像不能嵌套，会crash，所以先把这个复制一份，然后先ReleaseStringUTFChars
	// linyehui 2014-04-22
	int len = strlen(pHashString);
	char* hash_buffer = new char[len+1];
	if (0 == hash_buffer)
	{
		env->ReleaseStringUTFChars(hash_string, pHashString);
		return -1;
	}
	strcpy(hash_buffer, pHashString);
	env->ReleaseStringUTFChars(hash_string, pHashString);

	jboolean isCopy = false;
	jbyteArray pArray = (jbyteArray) env->GetPrimitiveArrayCritical(buffer, &isCopy);
	jsize size = env->GetArrayLength(buffer);

	jint output_len = genWaveData(hash_buffer, (char*)pArray, size);

	delete hash_buffer;
	env->ReleasePrimitiveArrayCritical(buffer, pArray, JNI_COMMIT);

	return output_len;
}

#ifdef __cplusplus
}
#endif
