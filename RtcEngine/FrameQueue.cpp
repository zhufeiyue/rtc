#include "FrameQueue.h"

FrameQueue::~FrameQueue()
{
	m_emptyFrameQueue.consume_all([](Frame* pFrame)
		{
			if (pFrame)
			{
				delete pFrame;
			}
		});

	m_usedFrameQueue.consume_all([](Frame* pFrame)
		{
			if (pFrame)
			{
				delete pFrame;
			}
		});
}

FrameQueue::Frame* FrameQueue::PopEmptyFrame(int w, int h, int size)
{
	Frame* pFrame(NULL);

	if ((w != m_iFrameWidth || h != m_iFrameHeight) || !m_emptyFrameQueue.pop(pFrame))
	{
		try
		{
			pFrame = new Frame();
			pFrame->height = h;
			pFrame->width = w;
			pFrame->pData = new unsigned char[size];
			//LOG() << "alloce new frame";
		}
		catch (...)
		{
			if (pFrame)
				delete pFrame;
			return NULL;
		}
	}

#ifdef _WIN32
	pFrame->timestamp = GetTickCount64();
#endif

	return pFrame;
}

void FrameQueue::PushEmptyFrame(Frame* pFrame)
{
	if (!pFrame)
	{
		return;
	}

	if (pFrame->height != m_iFrameHeight || pFrame->width != m_iFrameWidth)
	{
		delete pFrame;
		return;
	}

	if (!m_emptyFrameQueue.push(pFrame))
	{
		delete pFrame;
	}
}

FrameQueue::Frame* FrameQueue::PopUsedFrame()
{
	Frame* pFrame = NULL;

	if (!m_usedFrameQueue.pop(pFrame))
	{
		return NULL;
	}

	return pFrame;
}

void FrameQueue::PushUsedFrame(Frame* pFrame)
{
	if (!pFrame)
	{
		return;
	}

	if (!m_usedFrameQueue.push(pFrame))
	{
		delete pFrame;
	}
}