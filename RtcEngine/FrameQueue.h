#pragma once
#include <boost/lockfree/queue.hpp>
#include "../common/common.h"

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

template <int s1, int s2>
class UndefinedFrameSizeQueue
{
public:
	struct Frame
	{
		int usedSize = 0;
		int maxSize = 0;
		unsigned char* pData = 0;
		long long timestamp = 0;

		~Frame()
		{
			if (pData)
				delete[] pData;
		}
	};

	UndefinedFrameSizeQueue()
	{
		std::enable_if_t< (s1 >= 1024 && s2 > s1 ), int> n = 10;
	}

	~UndefinedFrameSizeQueue()
	{
		m_usedFQ.consume_all([](Frame* pFrame) { if (pFrame) delete pFrame; });
		m_emptyFQ1.consume_all([](Frame* pFrame) { if (pFrame) delete pFrame; });
		m_emptyFQ2.consume_all([](Frame* pFrame) { if (pFrame) delete pFrame; });
		m_emptyFQ3.consume_all([](Frame* pFrame) { if (pFrame) delete pFrame; });
	}

	Frame* PopEmptyFrame(int size)
	{
		Frame* pFrame = NULL;
		int allocSize = 0;

		if (size <= s1)
		{
			m_emptyFQ1.pop(pFrame);
			allocSize = s1;
		}
		else if (size <= s2)
		{
			m_emptyFQ2.pop(pFrame);
			allocSize = s2;
		}
		else
		{
			m_emptyFQ3.pop(pFrame);
			if (pFrame)
				if (pFrame->maxSize < size)
				{
					delete pFrame;
					pFrame = NULL;
					allocSize = size / 4096 * 4096 + 4096;
				}
		}

		if (!pFrame)
		{
			try
			{
				pFrame = new Frame();
				pFrame->maxSize = allocSize;
				pFrame->pData = new unsigned char[allocSize];
			}
			catch (...)
			{
				LOG() << "fail to alloc new UndefinedFrame " << size;
				if (pFrame)
					delete pFrame;
				return NULL;
			}
		}

		pFrame->usedSize = size;
#ifdef _WIN32
		pFrame->timestamp = GetTickCount64();
#endif
		return pFrame;
	}

	void PushEmptyFrame(Frame* pFrame)
	{
		if (!pFrame)
			return;

		if (pFrame->maxSize <= s1)
		{
			if (!m_emptyFQ1.push(pFrame))
				delete pFrame;
		}
		else if (pFrame->maxSize <= s2)
		{
			if (!m_emptyFQ2.push(pFrame))
				delete pFrame;
		}
		else if (!m_emptyFQ3.push(pFrame))
		{
			delete pFrame;
		}
	}

	Frame* PopUsedFrame()
	{
		Frame* pFrame = NULL;

		if (!m_usedFQ.pop(pFrame))
		{
			return NULL;
		}

		return pFrame;
	}

	void PushUsedFrame(Frame* pFrame)
	{
		if (!pFrame)
		{
			return;
		}

		if (!m_usedFQ.push(pFrame))
		{
			LOG() << "fail to push used frame " << pFrame->maxSize << ',' << pFrame->usedSize;
			delete pFrame;
		}
	}

private:
	boost::lockfree::queue<Frame*,
		boost::lockfree::capacity<128>,
		boost::lockfree::fixed_sized<true>> m_usedFQ;
	boost::lockfree::queue<Frame*,
		boost::lockfree::capacity<64>,
		boost::lockfree::fixed_sized<true>> m_emptyFQ1, m_emptyFQ2, m_emptyFQ3;
};