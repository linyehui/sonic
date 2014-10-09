package com.duowan.sonic;

import android.util.Log;

public class SonicListener {
    public static native int initFFTMgr(long maxFPS);
    public static native void releaseFFTMgr();
    public static native int grabAudioData(long inNumberFrames, long numberChannels, long  dataByteSize, byte[] data);
    public static native boolean computeWave(byte[] code);
    
    public static void preLoad(){  
        Log.i("SonicListener", "PreLoad");  
    }  

    static 
    {
    	Log.i("SonicListener","before load library");
    	// 注意这里为自己指定的.so文件，无lib前缀，亦无后缀
        System.loadLibrary("sonicengine");
        Log.i("SonicListener","after load library");  
    }
}
