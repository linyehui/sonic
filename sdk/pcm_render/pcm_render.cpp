//
//  pcm_render.cpp
//
//
//  Created by linyehui on 2014-03-06.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#include "pcm_render.h"
#include "bb_header.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define DURATION                0.0872 // seconds 0.1744//
#define MAX_VOLUME              0.5
#define MAX_SHORT               32767
#define MIN_SHORT               -32768
#define CHANNELS_NUM            1

#define WAVE_FORMAT_PCM         1
#define WAVE_FORMAT_IEEE_FLOAT  0x0003  /*  Microsoft Corporation  */

const int BYTE_PER_SAMPLE = sizeof(short);

//////////////////////////////////////////////////////////////////////////

#pragma mark -

//wav文件格式详见：http://www-mmsp.ece.mcgill.ca/Documents../AudioFormats/WAVE/WAVE.html
//wav头的结构如下所示：
typedef   struct   {
    char         fccID[4];//"RIFF"标志
    unsigned   long       dwSize;//文件长度
    char         fccType[4];//"WAVE"标志
}HEADER;

typedef   struct   {
    char         fccID[4];//"fmt"标志
    unsigned   long       dwSize;//Chunk size: 16
    unsigned   short     wFormatTag;// 格式类别
    unsigned   short     wChannels;//声道数
    unsigned   long       dwSamplesPerSec;//采样频率
    unsigned   long       dwAvgBytesPerSec;//位速  sample_rate * 2 * chans//为什么乘2呢？因为此时是16位的PCM数据，一个采样占两个byte。
    unsigned   short     wBlockAlign;//一个采样多声道数据块大小
    unsigned   short     uiBitsPerSample;//一个采样占的bit数
}FMT;

typedef   struct   {
    char         fccID[4]; 	//数据标记符＂data＂
    unsigned   long       dwSize;//语音数据的长度，比文件长度小36
}DATA;

//添加wav头信息
int addWAVHeader(unsigned char *buffer, int sample_rate, int BYTE_PER_SAMPLE, int channels, long dataByteSize)
{
    //以下是为了建立.wav头而准备的变量
    HEADER   pcmHEADER;
    FMT   pcmFMT;
    DATA   pcmDATA;
    
    //以下是创建wav头的HEADER;但.dwsize未定，因为不知道Data的长度。
//    strcpy(pcmHEADER.fccID,"RIFF");
    memcpy(pcmHEADER.fccID, "RIFF", sizeof(char)*4);
    pcmHEADER.dwSize = SonicSDK::SONIC_HEADER_SIZE + dataByteSize;   //根据pcmDATA.dwsize得出pcmHEADER.dwsize的值
    memcpy(pcmHEADER.fccType, "WAVE", sizeof(char)*4);
    
    memcpy(buffer, &pcmHEADER, sizeof(pcmHEADER));
    
    //以上是创建wav头的HEADER;
    
    //以下是创建wav头的FMT;
//    strcpy(pcmFMT.fccID,"fmt ");
    memcpy(pcmFMT.fccID, "fmt ", sizeof(char)*4);
    pcmFMT.dwSize=16;
    pcmFMT.wFormatTag=WAVE_FORMAT_PCM;
    pcmFMT.wChannels=channels;
    pcmFMT.dwSamplesPerSec=sample_rate;
    pcmFMT.dwAvgBytesPerSec=sample_rate * BYTE_PER_SAMPLE * channels;//F * M * Nc
    pcmFMT.wBlockAlign=BYTE_PER_SAMPLE * channels;//M * Nc
    pcmFMT.uiBitsPerSample=ceil(8 * BYTE_PER_SAMPLE);
    
    memcpy(buffer+sizeof(pcmHEADER), &pcmFMT, sizeof (pcmFMT));
    //以上是创建wav头的FMT;
    
    //以下是创建wav头的DATA;   但由于DATA.dwsize未知所以不能写入.wav文件
//    strcpy(pcmDATA.fccID,"data");
    memcpy(pcmDATA.fccID, "data", sizeof(char)*4);
    pcmDATA.dwSize=dataByteSize; //给pcmDATA.dwsize   0以便于下面给它赋值
    
    memcpy(buffer+sizeof(pcmHEADER)+sizeof(pcmFMT), &pcmDATA, sizeof(pcmDATA));
    
    return 0;
}

////添加wav头信息
//int addCAFHeader(unsigned char *buffer, int sample_rate, int bytesPerSample, int channels, long dataByteSize)
//{
//    //以下是为了建立.wav头而准备的变量
//    CAFFileHeader   fileHEADER;
//    CAFChunkHeader   chunkHEADER;
//    CAFAudioFormat  audioFormatHEADER;
//    
//    
//    strcpy(fileHEADER.mFileType,"caff");
//    fileHEADER.mFileVersion = 1;
//    fileHEADER.mFileFlags = 0;
//    
//    strcpy(chunkHEADER.mChunkType,"desc");
//    chunkHEADER.mChunkSize = sizeof(CAFAudioFormat);
//    
//    audioFormatHEADER.mSampleRate = 44100;
//    strcpy(audioFormatHEADER.mFormatID,"lpcm");
//    audioFormatHEADER.mFormatFlags = (1L << 0);
//    audioFormatHEADER.mBytesPerPacket =
//    audioFormatHEADER
//    
//
//    
//    return 0;
//}

//添加caf头信息
/*
int addCAFHeader(unsigned char *buffer, int sample_rate, int bytesPerSample, int channels, long dataByteSize)
{
    NSMutableData *headerData = [NSMutableData data];
    // caf header
    CAFFileHeader ch = {kCAF_FileType, kCAF_FileVersion_Initial, 0};
    ch.mFileType = CFSwapInt32HostToBig(ch.mFileType);
    ch.mFileVersion = CFSwapInt16HostToBig(ch.mFileVersion);
    ch.mFileFlags = CFSwapInt16HostToBig(ch.mFileFlags);
//    write(fd, &ch, sizeof(CAFFileHeader));
    [headerData appendBytes:&ch length:sizeof(CAFFileHeader)];
    
    // audio description chunk
    CAFChunkHeader cch;
    cch.mChunkType = CFSwapInt32HostToBig(kCAF_StreamDescriptionChunkID);
    cch.mChunkSize = sizeof(CAFAudioDescription);
    cch.mChunkSize = CFSwapInt64(cch.mChunkSize);
//    write(fd, &cch.mChunkType, sizeof(cch.mChunkType));
    [headerData appendBytes:&cch.mChunkType length:sizeof(cch.mChunkType)];
//    write(fd, &cch.mChunkSize, sizeof(cch.mChunkSize));
    [headerData appendBytes:&cch.mChunkSize length:sizeof(cch.mChunkSize)];
    
    // CAFAudioDescription
    CAFAudioDescription cad;
    CFSwappedFloat64 swapped_sr = CFConvertFloat64HostToSwapped(sample_rate);
    cad.mSampleRate = *((Float64*)(&swapped_sr.v));
    cad.mFormatID = CFSwapInt32HostToBig(kAudioFormatLinearPCM);
    cad.mFormatFlags = 0;
    cad.mFormatFlags |= kCAFLinearPCMFormatFlagIsFloat;
    cad.mFormatFlags = CFSwapInt32HostToBig(cad.mFormatFlags);

    cad.mBytesPerPacket = CFSwapInt32HostToBig(bytesPerSample);
    cad.mFramesPerPacket = CFSwapInt32HostToBig(channels);
    cad.mChannelsPerFrame = CFSwapInt32HostToBig(channels);
    cad.mBitsPerChannel = CFSwapInt32HostToBig(bytesPerSample);
//    write(fd, &cad, sizeof(CAFAudioDescription));
    [headerData appendBytes:&cad length:sizeof(CAFAudioDescription)];
    
    // audio data chunk
    cch.mChunkType = CFSwapInt32HostToBig(kCAF_AudioDataChunkID);
    cch.mChunkSize = (SInt64)CFSwapInt64HostToBig(dataByteSize + sizeof(UInt32));
//    write(fd, &cch.mChunkType, sizeof(cch.mChunkType));
    [headerData appendBytes:&cch.mChunkType length:sizeof(cch.mChunkType)];
//    write(fd, &cch.mChunkSize, sizeof(cch.mChunkSize));
    [headerData appendBytes:&cch.mChunkSize length:sizeof(cch.mChunkSize)];
    
    
//    // audio data
//    UInt32 edit_count = 0;
//    write(fd, &edit_count, sizeof(UInt32));
//    write(fd, samples, samples_size);
    
//    // flush to disk
//    close(fd);
//    
//    // free the samples buffer
//    free(samples);
    
    return 0;
}
*/

#pragma mark - 数字转频率

void makeChirp(short buffer[],int bufferLength,unsigned int freqArray[], int freqArrayLength, double duration_secs,
               long sample_rate, int bits_persample, int channels) {
    
    double theta = 0;
    int idx = 0;
    for (int i=0; i<freqArrayLength; i++) {
        
        double theta_increment = 2.0 * M_PI * freqArray[i] / sample_rate;
        
        // Generate the samples
        for (unsigned int frame = 0; frame < (duration_secs * sample_rate); frame++)
//        for (UInt32 frame = 0; frame < (bufferLength / freqArrayLength); frame++)
        {
            float vol = MAX_VOLUME * sqrt( 1.0 - (pow(frame - ((duration_secs * sample_rate) / 2), 2)
                                                    / pow(((duration_secs * sample_rate) / 2), 2)));
            
            // 因为Android不支持WAVE_FORMAT_IEEE_FLOAT 32Bit Float格式，只支持到16Bit的WAVE_FORMAT_PCM
            // 所以这里把生成的float*32767，从32Bit的float转成16Bit的short
            // linyehui 2014-04-02 15:00
            float frame_value_float = vol * sin(theta);
            float frame_value_16 = MAX_SHORT * frame_value_float;
            if( frame_value_16 > MAX_SHORT ) frame_value_16 = MAX_SHORT;
            if( frame_value_16 < MIN_SHORT ) frame_value_16 = MIN_SHORT;
            
            if (idx < bufferLength)
            {
                buffer[idx++] = (short)frame_value_16;
                if(channels == 2)
                    buffer[idx++] = (short)frame_value_16;
            }
            
            theta += theta_increment;
            if (theta > 2.0 * M_PI)
            {
                theta -= 2.0 * M_PI;
            }
        }
        
    }
}

/*
+ (BOOL)isHighFreq {
    
    return !!freq_init_is_high;
}

+ (void)switchFreq:(BOOL)isHigh {

    int is_high = (isHigh ? 1 : 0);
    switch_freq(is_high);
}
*/

int getChirpLength(unsigned int data_len)
{
    // 定义buffer总长度
    // 所有频率总长度(包括17，19)
    int bufferLength = (int)(DURATION * SAMPLE_RATE * (data_len+2) * CHANNELS_NUM);
    return bufferLength;
}

int SonicSDK::PCMRender::getChirpLengthByByte(unsigned int data_len)
{
	int chirp_len = getChirpLength(data_len);
	int len_by_byte = chirp_len * sizeof(short);

    return len_by_byte;
}

bool SonicSDK::PCMRender::renderChirpData(
                                        const char* code_with_rs,
                                        unsigned int data_len,
                                        unsigned char* header,
                                        unsigned int header_len,
                                        short* wave_buffer,
                                        long buffer_len_by_byte)
{
    if (NULL == wave_buffer || data_len <= 0)
        return false;
    if (NULL == header || SONIC_HEADER_SIZE != header_len)
        return false;
    
    /*
     *  序列化字符串转频率
     */
    char* charArray = (char*)code_with_rs;
    //unsigned short* charArray = (unsigned short*)malloc(sizeof(unsigned short)*data_len);
    //memcpy(charArray, code_with_rs, sizeof(unsigned short)*data_len);
    
    unsigned freqArray[data_len+2];//起始音17，19
    //memset(freqArray, 0, sizeof(unsigned) * (serializeStr.length+2));
    
    char_to_freq('h', freqArray);
    char_to_freq('j', freqArray+1);
    
    //freqArray[0] = 123;
    //freqArray[1] = 321;
    
    for (int i=0; i<data_len; i++) {
        
        //unsigned int freq = 0;
        char_to_freq(charArray[i], freqArray+i+2);
        //freqArray[i+2] = freq;
    }
    
     for (int i=0; i < 20; i++)
     {
         printf("%d\r\n", freqArray[i]);
     }
    
    //free(charArray);
    
    //定义buffer总长度
    int bufferLength = getChirpLength(data_len);
    if (buffer_len_by_byte < sizeof(short) * bufferLength)
        return false;
    
    memset(wave_buffer, 0, buffer_len_by_byte);
    
    makeChirp(wave_buffer, bufferLength, freqArray, data_len+2, DURATION, SAMPLE_RATE, 8 * BYTE_PER_SAMPLE, CHANNELS_NUM);
    
    //unsigned char wavHeaderByteArray[WAV_HEADER_SIZE];
    memset(header, 0, SONIC_HEADER_SIZE);
    
    addWAVHeader(header, SAMPLE_RATE, BYTE_PER_SAMPLE, CHANNELS_NUM, sizeof(short) * bufferLength);
    
//    NSMutableData *chirpData = [[NSMutableData alloc] initWithBytes:wavHeaderByteArray length:sizeof(wavHeaderByteArray)];
//    [chirpData appendBytes:buffer length:sizeof(buffer)];
//    
//    return [chirpData autorelease];
    
    return true;
}
