#include "IEncoder.h"

IEncoder::~IEncoder()
{
	if (m_threadWorker.joinable())
	{
		m_threadWorker.join();
	}
}

void IEncoder::SetEncoderConfig(EncoderConfig ec)
{
	m_encoderConfig = ec;
}

EncoderConfig IEncoder::GetEncoderConfig()
{
	return m_encoderConfig;
}