package com.duowan.sonic;

import android.app.Activity;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity {

	// 音频获取源  
    private int audioSource = MediaRecorder.AudioSource.MIC;  
    // 设置音频采样率，44100是目前的标准，但是某些设备仍然支持22050，16000，11025  
    private static int sampleRateInHz = 44100;  
    
    // 设置音频的录制的声道CHANNEL_IN_STEREO为双声道，CHANNEL_CONFIGURATION_MONO为单声道  
    private static int channelConfig = AudioFormat.CHANNEL_IN_MONO; 
    private static int CHANNEL_COUNT = 1;
    
    // 音频数据格式:PCM 16位每个样本。保证设备支持。PCM 8位每个样本。不一定能得到设备支持。  
    private static int audioFormat = AudioFormat.ENCODING_PCM_16BIT;  
    private static int BYTES_PER_FRAME = 2; // short==16bit==2byte
    
    // 缓冲区字节大小  
	private static int DEFAULT_BUFFER_SIZE = 4096 * CHANNEL_COUNT * BYTES_PER_FRAME;
	private int inNumberFrames = 0;
	
    private int bufferSizeInBytes = 0;  
    private AudioRecord audioRecord;  
    private boolean isRecording = false;
	private boolean isListening = false;
		
    public MainActivity()  
    {  
        Log.i("MainActivity", "Constructor");  
    }
    
 	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		SonicGenerator.preLoad();
		SonicListener.preLoad();
		createAudioRecord(); 
	}

	@Override  
    protected void onDestroy() {  
		isListening = false;
		stopRecord();
		close();
		SonicListener.releaseFFTMgr();
        super.onDestroy();  
    }  
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	} 
	
    private void createAudioRecord() {  
        try {
        	// 获得缓冲区字节大小  
            bufferSizeInBytes = AudioRecord.getMinBufferSize(sampleRateInHz,  
                    channelConfig, audioFormat);  
            if (bufferSizeInBytes < DEFAULT_BUFFER_SIZE)
            	bufferSizeInBytes = DEFAULT_BUFFER_SIZE;
            
            // 创建AudioRecord对象  
            audioRecord = new AudioRecord(audioSource, sampleRateInHz,  
                    channelConfig, audioFormat, bufferSizeInBytes);  
            
            if (audioRecord.getState() == AudioRecord.STATE_INITIALIZED)
            {
                Log.d("Recorder", "Audio recorder initialised successed");
                
                inNumberFrames = bufferSizeInBytes / BYTES_PER_FRAME / CHANNEL_COUNT;
                SonicListener.initFFTMgr(inNumberFrames);
                
                startRecord();
            }
            else
            {
            	Log.d("Recorder", "Audio recorder initialised failed");
            }
        }
        catch (IllegalArgumentException e) {
        	Log.d("Recorder", "Exception");
        }
    	
    }
    
	public void onClickGenerate(View v) {
		String hash = "nk104a5dj8";
		int buffer_len = SonicGenerator.getWaveLenByByte();
		byte[] buffer = new byte[buffer_len];
		
		SonicGenerator.genWaveData(hash, buffer);
		playShortAudioFileViaAudioTrack(buffer);
		
		Log.d("Click", "Generate");
	} 
	
	public void onClickStartListening(View v) {
	    isListening = true;
		Log.d("Click", "StartListening");
	} 
	
	public void onClickStopListening(View v) {
		isListening = false;
		Log.d("Click", "StopListening");
	} 
	
	private void playShortAudioFileViaAudioTrack(byte[] byteData) {
		// Set and push to audio track..
		int iMinBufferSize = android.media.AudioTrack.getMinBufferSize(sampleRateInHz, AudioFormat.CHANNEL_OUT_MONO,
		AudioFormat.ENCODING_PCM_16BIT); 		
		AudioTrack at = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRateInHz, AudioFormat.CHANNEL_OUT_MONO,
		AudioFormat.ENCODING_PCM_16BIT, iMinBufferSize, AudioTrack.MODE_STREAM); 
		
		at.play();
		at.write(byteData, 0, byteData.length); 
		at.stop();
		at.release();
		at = null;
	}
		
	private void startRecord() {
		if (isRecording)
			return;
		
        audioRecord.startRecording();  
        isRecording = true;  
        
        new Thread(new AudioRecordThread()).start();  
        new Thread(new AudioAnalysisThread()).start();
}  
  
    private void stopRecord() {  
    	if (audioRecord != null) {
    		isRecording = false;  
            audioRecord.stop();  
    	}
    }  
  
    private void close() {  
        if (audioRecord != null) {  
            isRecording = false;
            audioRecord.stop();  
            audioRecord.release(); 
            audioRecord = null;  
        }
    }  
  
    class AudioRecordThread implements Runnable {  
        @Override  
        public void run() {  
        	performThru();   	 
        }  
    }
    
     private void performThru() {  
        byte[] audiodata = new byte[bufferSizeInBytes];  
        int readsize = 0;  
        while (isRecording == true) {  
            readsize = audioRecord.read(audiodata, 0, bufferSizeInBytes);        	
            if (AudioRecord.ERROR_INVALID_OPERATION != readsize) {
            	/* int result = */
            	SonicListener.grabAudioData(
            			inNumberFrames, 
            			CHANNEL_COUNT, 
            			readsize,
            			audiodata);
            	//Log.d("performThru", String.format("result = %d", result));
            }  
        }  
    }
     
	 class AudioAnalysisThread implements Runnable {  
	     @Override  
	     public void run() {
	    	 
	 		// 20Hz
			 long timeInterval = (long)((1 / 20.) * 1000);
				while(true)
				{
					 try {
						 Thread.sleep(timeInterval);
					    } 
					 catch (InterruptedException e) {
					        e.printStackTrace(); 
					    }
					 
					computeWave();
				}
	     }  
	 }
	
	 private void computeWave() {
		if (!isListening)
			return;
		
		byte[] result_code = new byte[10]; 
		if (!SonicListener.computeWave(result_code))
		{
			return;
		}
		
		String msg = new String(result_code);
		Log.d("computeWave", msg);
		showToast(msg);
	 }
	 
	 public void showToast(final String msg) {
		 new Handler(Looper.getMainLooper()).post(
				 new Runnable() {

					@Override
					public void run() {
						Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
					}
				 });
	}
}
