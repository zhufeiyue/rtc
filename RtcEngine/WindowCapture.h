#pragma once
#include "../common/common.h"

class FrameQueue;
class WindowCapture
{
public:
	enum class WindowType
	{
		DesktopWindow = 1,
		NormalWindow
	};

	explicit WindowCapture(HWND);
	~WindowCapture();

	int SetFrameSize(int w, int h);
	virtual int Init();
	virtual int Destroy();

	virtual int CpatureFrame();
	virtual int SwitchWindow(HWND);

	FrameQueue* GetFrameQueue()
	{
		return m_frameQueue.get();
	}

	void SetWindowType(WindowType type)
	{
		m_iWindowType = type;
	}

protected:
	void CreateBitmap(int w, int h);
	void SaveBitmap();
	void SaveYUV();
	void CaptureCursor(const RECT&);

protected:
	HWND m_hWnd = NULL;
	HDC m_hDC = NULL;
	HDC m_hMemDC = NULL;
	HBITMAP m_hBitmap = NULL;
	RECT m_rectOLD = { 0 };
	LONG m_oldWidth = 0;  // ´°¿Ú¿í
	LONG m_oldHeight = 0; // ´°¿Ú¸ß

	unsigned char* pBmpData_ = NULL;
	std::unique_ptr<unsigned char> pBmpDataUpSideDown;
	std::unique_ptr<unsigned char> m_pYUVData;
	BITMAPINFOHEADER bitmapHeader_ = { 0 };
	std::unique_ptr<FrameQueue> m_frameQueue;
	int m_iFrameWidth = 0; 
	int m_iFrameHeight = 0;
	int m_iFrameSize = 0;
	UINT m_printWindowFlag = 0;
	WindowType m_iWindowType = WindowType::NormalWindow;
	bool m_bIsWin81OrGreater = true;
};