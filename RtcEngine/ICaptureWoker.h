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
