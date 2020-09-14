#include "ICaptureWoker.h"

ICaptureWoker::~ICaptureWoker()
{
	if (m_threadWorker.joinable())
	{
		m_threadWorker.join();
	}
}

int ICaptureWoker::SetVideoCaptureConfigure(CaptureConfigure vcc)
{
	m_captureConf = vcc;
	return S_OK;
}

CaptureConfigure ICaptureWoker::GetVideoCaptureConfigure() const
{
	return m_captureConf;
}