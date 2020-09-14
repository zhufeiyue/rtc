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
