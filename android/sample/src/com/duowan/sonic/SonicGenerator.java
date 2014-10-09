package com.duowan.sonic;

import android.util.Log;

public class SonicGenerator 
{
	public static native int getWaveLenByByte();
    public static native int genWaveData(String hash, byte[] wave_buffer);
    
    public static void preLoad()
    {  
        Log.i("SonicGenerator", "PreLoad");  
    }  

    static 
    {
    	Log.i("SonicGenerator","before load library");
    	// 注意这里为自己指定的.so文件，无lib前缀，亦无后缀
        System.loadLibrary("sonicengine");
        Log.i("SonicGenerator","after load library");  
    }
}
