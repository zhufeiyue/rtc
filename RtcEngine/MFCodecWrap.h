#pragma once
#include "common.h"
#include <mftransform.h>

class MFH264Encoder
{
public:
	MFH264Encoder();
	~MFH264Encoder();

	void SetFrameSize(int, int, int);
	int Init();
	int Destroy();
	int ProcessInput(uint8_t* [], int);

protected:
	IMFTransform* m_pEncoder = NULL;
	IMFSample* m_pInputSample = NULL;
	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_iFrameRate = 0;
	std::string m_strSequenceHeader;
	DWORD m_dwInStreamID = 0;
	DWORD m_dwOutStreamID = 0;
	BOOL m_bYV12 = false;
	MFT_INPUT_STREAM_INFO m_streamInfoIn = { 0 };
	MFT_OUTPUT_STREAM_INFO m_streamInfoOut = { 0 };
	MFT_OUTPUT_DATA_BUFFER m_dataOut = { 0 };
	INT64 m_iSampleDuration = 0;
	INT64 m_iSampleTime = 0;
	INT64 m_iInputSampleCount = 0;
};

class MFAACEncoder
{
public:
	struct AudioSpecificConfig
	{
		unsigned char aac_seq_head_audioObjectType;        // 5 bits; 1 aac main, 2 aac lc,  
		unsigned char aac_seq_head_samplingFrequencyIndex; // 4 bits; 0 96000, 1 88200, 2 64000, 3 48000, 4 44100, 5 32000, 16 取决于samplingFrequency
		unsigned int aac_seq_head_samplingFrequency;       // 24 bits;
		unsigned char aac_seq_head_channelConfiguration;   // 4 bits; 声道信息
	};
	static void ParseAudioSpecificConfig(AudioSpecificConfig&, const unsigned char*, int);
	static void CreateAACADTSHeader(unsigned char * pRecvBuf, const AudioSpecificConfig& , int aacDataLen);

public:
	MFAACEncoder();
	~MFAACEncoder();

	void SetInputSampleInfo(int fr, int channel, int bits);
	int Init();
	int Destroy();
	int ProcessInput(uint8_t* [], int);
	int ProcessInput(IMFSample* pSample);

protected:
	int m_iInputSampleRate = 0;
	int m_iInputSampleChannel = 0;
	int m_iInputSampleBits = 0;
	int m_iOutputBytePerSecond = 12000;
	IMFTransform* m_pEncoder = NULL;
	MFT_INPUT_STREAM_INFO m_streamInfoIn = { 0 };
	MFT_OUTPUT_STREAM_INFO m_streamInfoOut = { 0 };
	MFT_OUTPUT_DATA_BUFFER m_dataOut = { 0 };
	AudioSpecificConfig m_aacAudioConfig = { 0 };
};