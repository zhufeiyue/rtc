#pragma once
#include "ICaptureWoker.h"
#include "WinCapture.h"

class CaptureWindowWorker : public ICaptureWoker
{
public:
	FrameQueue* GetFrameQueue()
	{
		if (m_pWinCapture)
			return m_pWinCapture->GetFrameQueue();
		else
			return NULL;
	}

	void SetHWND(HWND hWnd);
	HWND GetHWND();
	void SetWindowType(WinCapture::WindowType);
	WinCapture::WindowType GetWindowType();

	int Init() override;
	int Destroy() override;

protected:
	int WorkLoop() override;
	int DoCapture() override;

private:
	HWND m_hWnd = NULL;
	WinCapture::WindowType m_iWindowType = WinCapture::WindowType::NormalWindow;
	std::unique_ptr<WinCapture> m_pWinCapture;
};
