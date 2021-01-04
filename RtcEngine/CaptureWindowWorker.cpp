#include "CaptureWindowWorker.h"

void CaptureWindowWorker::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
}

HWND CaptureWindowWorker::GetHWND()
{
	return m_hWnd;
}

void CaptureWindowWorker::SetWindowType(WindowCapture::WindowType type)
{
	m_iWindowType = type;
}

WindowCapture::WindowType CaptureWindowWorker::GetWindowType()
{
	return m_iWindowType;
}

int CaptureWindowWorker::Init()
{
	if (!m_hWnd || !IsWindow(m_hWnd))
	{
		return CodeFalse;
	}

	if (m_bRunning)
	{
		return CodeFalse;
	}

	if (m_threadWorker.joinable())
	{
		m_threadWorker.join();
	}

	auto vcc = GetVideoCaptureConfigure();
	m_pWindowCapture.reset(new WindowCapture(m_hWnd));
	m_pWindowCapture->SetWindowType(m_iWindowType);
	m_pWindowCapture->SetFrameSize(vcc.width, vcc.height);
	if (CodeOK != m_pWindowCapture->Init())
	{
		LOG() << "fail to init window capture";
		return CodeFalse;
	}

	m_bRunning = true;
	m_threadWorker = std::thread([this]() { this->WorkLoop(); });

	return CodeOK;
}

int CaptureWindowWorker::Destroy()
{
	m_bRunning = false;
	m_threadWorker.join();

	if (m_pWindowCapture)
	{
		m_pWindowCapture->Destroy();
		m_pWindowCapture.reset();
	}

	return CodeOK;
}

int CaptureWindowWorker::WorkLoop()
{
	if (!m_pWindowCapture)
	{
		return CodeFalse;
	}

	WaitSomeTime wst(1000000000 / GetVideoCaptureConfigure().frameRate);
	while (m_bRunning)
	{
		wst.Wait();
		DoCapture();
	}

	return CodeOK;
}

int CaptureWindowWorker::DoCapture()
{
	if (!m_pWindowCapture)
	{
		return CodeFalse;
	}

	auto res = m_pWindowCapture->CpatureFrame();
	if (res != CodeOK)
	{
		return res;
	}

	return CodeOK;
}