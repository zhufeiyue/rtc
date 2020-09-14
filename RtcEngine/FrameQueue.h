#pragma once
#include <boost/lockfree/queue.hpp>

class FrameQueue
{
public:
	struct Frame
	{
		int width;
		int height;
		unsigned char* pData;
		long long timestamp;

		Frame()
		{
			height = 0;
			width = 0;
			pData = NULL;
			timestamp = 0;
		}

		~Frame()
		{
			if (pData)
			{
				delete[] pData;
			}
		}
	};

public:
	~FrameQueue();

	int SetFrameSize(int w, int h)
	{
		m_iFrameWidth = w;
		m_iFrameHeight = h;
		return 0;
	}

	Frame* PopEmptyFrame(int w, int h, int size);
	void PushEmptyFrame(Frame*);

	Frame* PopUsedFrame();
	void PushUsedFrame(Frame*);

private:
	int m_iFrameWidth = 0;
	int m_iFrameHeight = 0;

	boost::lockfree::queue<Frame*,
		boost::lockfree::capacity<64>,
		boost::lockfree::fixed_sized<true>> m_emptyFrameQueue, m_usedFrameQueue;
};

