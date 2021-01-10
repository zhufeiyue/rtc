#pragma once
#include "../common/common.h"

struct CaptureConfigure
{
	int width = 1920;
	int height = 1080;
	int frameRate = 15;
	int frameFormat = 0;
};

class ICaptureWoker
{
public:
	virtual ~ICaptureWoker()
	{
		m_bRunning = false;
		if (m_threadWorker.joinable())
		{
			m_threadWorker.join();
		}
	}
	virtual int Init() = 0;
	virtual int Destroy() = 0;

	virtual int SetVideoCaptureConfigure(CaptureConfigure vcc)
	{
		m_captureConf = vcc;
		return S_OK;
	}
	virtual CaptureConfigure GetVideoCaptureConfigure() const
	{
		return m_captureConf;
	}
protected:
	virtual int WorkLoop() = 0;
	virtual int DoCapture() = 0;

protected:
	int m_bRunning = false;
	std::thread m_threadWorker;
	CaptureConfigure m_captureConf;
};
