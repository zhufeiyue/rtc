#pragma once
#include "../common/common.h"

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
	virtual ~IEncoder()
	{
		m_bRunning = false;
		if (m_threadWorker.joinable())
		{
			m_threadWorker.join();
		}
	}
	virtual int Init() = 0;
	virtual int Destroy() = 0;

	virtual void SetEncoderConfig(EncoderConfig ec)
	{
		m_encoderConfig = ec;
	}
	virtual EncoderConfig GetEncoderConfig()
	{
		return m_encoderConfig;
	}

protected:
	int m_bRunning = false;
	std::thread m_threadWorker;
	EncoderConfig m_encoderConfig;
};