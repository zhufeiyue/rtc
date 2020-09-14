#pragma once
#include "common.h"

struct CaptureConfigure
{
	int width = 1280;
	int height = 720;
	int frameRate = 15;
	int frameFormat = 0;
};

class ICaptureWoker
{
public:
	virtual ~ICaptureWoker();
	virtual int Init() = 0;
	virtual int Destroy() = 0;

	virtual int SetVideoCaptureConfigure(CaptureConfigure);
	virtual CaptureConfigure GetVideoCaptureConfigure()const;
protected:
	virtual int WorkLoop() = 0;
	virtual int DoCapture() = 0;

protected:
	int m_bRunning = false;
	std::thread m_threadWorker;
	CaptureConfigure m_captureConf;
};
