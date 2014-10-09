#  Created by linyehui on 2014-04-10.
#  Copyright (c) 2014年 linyehui. All rights reserved.
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../../sdk/ \
					$(LOCAL_PATH)/../../../sdk/FFTBufferManager/ \
					$(LOCAL_PATH)/../../../sdk/freq_util/ \
					$(LOCAL_PATH)/../../../sdk/iPublicUtility/ \
					$(LOCAL_PATH)/../../../sdk/kiss_fft/ \
					$(LOCAL_PATH)/../../../sdk/pcm_render/ \
					$(LOCAL_PATH)/../../../sdk/queue/ \
					$(LOCAL_PATH)/../../../sdk/rscode/ \
					$(LOCAL_PATH)/../../../sdk/generator_helper/ \
					$(LOCAL_PATH)/../../../sdk/listener_helper/

LOCAL_SRC_FILES := ../../../sdk/FFTBufferManager/FFTBufferManager.cpp \
                   ../../../sdk/freq_util/bb_freq_util.cpp \
                   ../../../sdk/iPublicUtility/VectorMath.cpp \
                   ../../../sdk/iPublicUtility/CADebugMacros.cpp \
                   ../../../sdk/iPublicUtility/CADebugPrintf.cpp \
                   ../../../sdk/iPublicUtility/CAStreamBasicDescription.cpp \
                   ../../../sdk/iPublicUtility/CAXException.cpp \
                   ../../../sdk/iPublicUtility/libkern/android/cutils/atomic.c \
                   ../../../sdk/iPublicUtility/libkern/android/OSAtomic.c \
                   ../../../sdk/kiss_fft/kiss_fft.c \
                   ../../../sdk/rscode/rscode.cpp \
                   ../../../sdk/queue/queue.cpp \
                   ../../../sdk/pcm_render/pcm_render.cpp \
                   ../../../sdk/generator_helper/generator_helper.cpp \
                   ../../../sdk/listener_helper/listener_helper.cpp \
                   SonicGenerator.cpp \
                   SonicListener.cpp \
                   DCRejectionFilter.cpp                 
                   
LOCAL_MODULE    := sonicengine
APP_OPTIM := debug

LOCAL_CFLAGS += -g -Wno-multichar
LOCAL_CFLAGS += -D SONIC_TARGET_OS_ANDROID
LOCAL_CFLAGS += -D ANDROID_SMP=0
          
# Include the static libraries pulled in via Android Maven plugin makefile (see include below)
LOCAL_STATIC_LIBRARIES := $(ANDROID_MAVEN_PLUGIN_LOCAL_STATIC_LIBRARIES)
          
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
            
include $(BUILD_SHARED_LIBRARY)

# Include the Android Maven plugin generated makefile
# Important: Must be the last import in order for Android Maven Plugins paths to work
include $(ANDROID_MAVEN_PLUGIN_MAKEFILE)