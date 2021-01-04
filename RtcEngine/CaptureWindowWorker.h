#pragma once
#include "ICaptureWoker.h"
#include "WindowCapture.h"

class CaptureWindowWorker : public ICaptureWoker
{
public:
	FrameQueue* GetFrameQueue()
	{
		if (m_pWindowCapture)
			return m_pWindowCapture->GetFrameQueue();
		else
			return NULL;
	}

	void SetHWND(HWND hWnd);
	HWND GetHWND();
	void SetWindowType(WindowCapture::WindowType);
	WindowCapture::WindowType GetWindowType();

	int Init() override;
	int Destroy() override;

protected:
	int WorkLoop() override;
	int DoCapture() override;

private:
	HWND m_hWnd = NULL;
	WindowCapture::WindowType m_iWindowType = WindowCapture::WindowType::NormalWindow;
	std::unique_ptr<WindowCapture> m_pWindowCapture;
};
