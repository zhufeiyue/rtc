#pragma once
#include "IEncoder.h"

template<typename FrameQueueType, typename EncoderType>
class H264Encoder: public IEncoder
{
public:
	int Init() override
	{
		if (!m_pFrameQueue)
		{
			return CodeFalse;
		}

		if (m_bRunning)
		{
			return CodeFalse;
		}

		m_bRunning = true;
		m_threadWorker = std::thread([this]()
			{
				WaitSomeTime wst(1000000000 / GetEncoderConfig().frameRate);
				FrameQueueType* pQueue = NULL;
				decltype(pQueue->PopUsedFrame())pFrame = NULL;
				uint8_t* yuv[3];
				auto ec = GetEncoderConfig();
				auto size = ec.width * ec.height;

				while (m_bRunning)
				{
					pQueue = GetFrameQueue();
					if (pQueue)
					{
						pFrame = pQueue->PopUsedFrame();
						if (pFrame)
						{
							yuv[0] = pFrame->pData;
							yuv[1] = pFrame->pData + size;
							yuv[2] = pFrame->pData + size / 4 + size;

							if (m_pEncoder)
							{
								m_pEncoder->ProcessInput(yuv, 0);
							}
							pQueue->PushEmptyFrame(pFrame);

							wst.Wait();
							continue;
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});

		return CodeOK;
	}

	int Destroy() override
	{
		m_bRunning = false;
		if (m_threadWorker.joinable())
			m_threadWorker.join();
		m_pEncoder = NULL;
		m_pFrameQueue = NULL;
		return CodeOK;
	}

	FrameQueueType* GetFrameQueue()
	{
		return m_pFrameQueue;
	}

	void SetFrameQueue(FrameQueueType* p)
	{
		m_pFrameQueue = p;
	}

	EncoderType* GetEncoder()
	{
		return m_pEncoder;
	}

	void SetEncoder(EncoderType* p)
	{
		m_pEncoder = p;
	}

protected:
	FrameQueueType* m_pFrameQueue = NULL;
	EncoderType* m_pEncoder = NULL;
};
