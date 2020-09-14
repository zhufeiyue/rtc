#pragma once
#include "common.h"

struct EncoderConfig
{
	int width = 0;
	int height = 0;
	int frameRate = 0;
	int frameFormat = 0;
};

class IEncoder
{
public:
	virtual ~IEncoder();
	virtual int Init() = 0;
	virtual int Destroy() = 0;

	virtual void SetEncoderConfig(EncoderConfig);
	virtual EncoderConfig GetEncoderConfig();

protected:
	int m_bRunning = false;
	std::thread m_threadWorker;
	EncoderConfig m_encoderConfig;
};