#include "WinCapture.h"
#include "FrameQueue.h"
#include "libyuv/libyuv.h"

#ifdef  _DEBUG
#pragma comment(lib, "./libyuv/yuvd.lib")
#else
#pragma comment(lib, "./libyuv/yuv.lib")
#endif //  _DEBUG

bool isWindows8Point1OrGreater()
{
	static bool *gsValue = NULL;
	if (gsValue)
	{
		return *gsValue;
	}

	typedef NTSTATUS  (__stdcall *pRtlGetVersion)(PRTL_OSVERSIONINFOW);

	bool res = false;
	pRtlGetVersion funcGetVersion = NULL;
	OSVERSIONINFOW versionInfo = { 0 };
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

	auto h = LoadLibraryA("ntdll");
	if (h)
	{
		funcGetVersion = (pRtlGetVersion)GetProcAddress(h, "RtlGetVersion");
		if (funcGetVersion)
		{
			funcGetVersion(&versionInfo);
			gsValue = new bool();
			res = versionInfo.dwMajorVersion >= 10 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion >= 3);
			*gsValue = res;
		}
		FreeLibrary(h);
	}


	return res;
}

WinCapture::WinCapture(HWND hwnd)
{
	m_hWnd = hwnd;
	if (!IsWindow(m_hWnd))
	{
	}
}

WinCapture::~WinCapture()
{
}

int WinCapture::SetFrameSize(int w, int h)
{
	BOOST_ASSERT_MSG(h >= 0 && h % 2 == 0, "video frame height must be an even number");
	BOOST_ASSERT_MSG(w >= 0 && w % 2 == 0, "video frame width must be an even number");
	m_iFrameWidth = w;
	m_iFrameHeight = h;
	m_iFrameSize = static_cast<int>(m_iFrameWidth * m_iFrameHeight * 1.5);
	return 0;
}

int WinCapture::Init()
{
	if (m_iFrameWidth == 0 || m_iFrameHeight == 0)
	{
		return CodeFalse;
	}

	m_hDC = GetWindowDC(m_hWnd);
	//m_hDC = GetDC(m_hWnd);

	if (isWindows8Point1OrGreater())
	{
		m_printWindowFlag = PW_CLIENTONLY | PW_RENDERFULLCONTENT;
		m_bIsWin81OrGreater = true;
	}
	else
	{
		m_printWindowFlag = PW_CLIENTONLY; // ok for wps, not for office 2016 show window in win10
	}

	if (!m_hDC)
	{
		return CodeFalse;
	}
	m_hMemDC = CreateCompatibleDC(m_hDC);
	if (!m_hMemDC)
	{
		return CodeFalse;
	}

	m_frameQueue.reset(new FrameQueue());
	if (!m_frameQueue)
	{
		return CodeFalse;
	}
	m_frameQueue->SetFrameSize(m_iFrameWidth, m_iFrameHeight);

	return CodeOK;
}

int WinCapture::Destroy()
{
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}

	if (m_hDC)
	{
		ReleaseDC(m_hWnd, m_hDC);
		m_hDC = NULL;
	}

	return CodeOK;
}

void WinCapture::CreateBitmap(int w , int h)
{
	memset(&bitmapHeader_, 0, sizeof(bitmapHeader_));
	bitmapHeader_.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader_.biPlanes = 1;
	bitmapHeader_.biHeight = h;
	bitmapHeader_.biWidth = w;
	bitmapHeader_.biSizeImage = bitmapHeader_.biHeight * bitmapHeader_.biWidth * 4;
	bitmapHeader_.biBitCount = 32;
	bitmapHeader_.biCompression = BI_RGB;
	m_hBitmap = CreateDIBSection(m_hDC, (BITMAPINFO*)&bitmapHeader_, DIB_RGB_COLORS, (void**)&pBmpData_, NULL, 0);
	SelectObject(m_hMemDC, m_hBitmap);

	//m_hBitmap = CreateCompatibleBitmap(m_hDC, w, h);
	//SelectObject(m_hMemDC, m_hBitmap);
	//bitmapHeader_.biSize = sizeof(BITMAPINFOHEADER);
	//auto res = GetDIBits(m_hMemDC, m_hBitmap, 0, h, NULL, (LPBITMAPINFO)&bitmapHeader_, DIB_RGB_COLORS);
	//pBmpData_ = new unsigned char[bitmapHeader_.biSizeImage];

	memset(pBmpData_, 255, bitmapHeader_.biSizeImage);
}

void WinCapture::SaveBitmap()
{
	static int count(0);

	BITMAPFILEHEADER bi = { 0 };

	bi.bfType = 0x4d42;
	bi.bfSize = sizeof(bi) + sizeof(bitmapHeader_) + bitmapHeader_.biSizeImage;
	bi.bfOffBits = sizeof(bi) + sizeof(bitmapHeader_);

	std::ofstream f;
	f.open(std::to_string(count++) + ".bmp", std::ofstream::binary | std::ofstream::out);
	if (f.is_open())
	{
		f.write((char*)&bi, sizeof(bi));
		f.write((char*)&bitmapHeader_, sizeof(bitmapHeader_));
		f.write((char*)pBmpData_, bitmapHeader_.biSizeImage);

		f.close();
	}
}

void WinCapture::SaveYUV()
{
	auto pYData = m_pYUVData.get();
	auto yLen = bitmapHeader_.biWidth * bitmapHeader_.biHeight;
	auto yStride = bitmapHeader_.biWidth;

	auto pUData = pYData + yLen;
	auto uLen = yLen / 4;
	auto uStride = yStride / 2;

	auto pVData = pUData + uLen;
	auto vLen = uLen;
	auto vStride = uStride;

	int res(0);
	int stride = bitmapHeader_.biWidth * 4;

	// 从HBITMAP获得的图像数据指针，指向图像的左下角，bgra格式

	// 旋转180度，虽然上下正确了，但会导致左右颠倒
	//res = libyuv::ARGBRotate(pBmpData_, bitmapHeader_.biWidth * 4,
	//	pBmpDataUpSideDown.get(), bitmapHeader_.biWidth * 4,
	//	bitmapHeader_.biWidth, bitmapHeader_.biHeight, libyuv::kRotate180);
	//if (res != 0)
	//{
	//	qDebug() << "ARGBRotate error " << res;
	//	return;
	//}

	// 水平镜像而非垂直镜像
	//res = libyuv::ARGBMirror(pBmpData_, stride,
	//	pBmpDataUpSideDown.get(), stride,
	//	bitmapHeader_.biWidth, bitmapHeader_.biHeight);

	for (int i = 0; i < bitmapHeader_.biHeight; ++i)
	{
		memcpy(pBmpDataUpSideDown.get() + i * stride,
			pBmpData_ + (bitmapHeader_.biHeight - 1 - i) * stride,
			stride);
	}

	res = libyuv::ARGBToI420(
		pBmpDataUpSideDown.get(),
		//pBmpData_,
		bitmapHeader_.biWidth * 4,
		pYData, yStride, 
		pUData, uStride, 
		pVData, vStride, 
		bitmapHeader_.biWidth,
		bitmapHeader_.biHeight);
	//qDebug() << "ARGBToI420 " << res;
	if (res != 0)
	{
		LOG() << "ARGBToI420 error " << res;
		return;
	}

	auto pFrame = m_frameQueue->PopEmptyFrame(m_iFrameWidth, m_iFrameHeight, m_iFrameSize);
	if (!pFrame)
	{
		LOG() << "PopEmptyFrame return NULL";
		return;
	}

	auto pDestYData = pFrame->pData;
	auto destYLen = pFrame->width * pFrame->height;
	auto desyStride = pFrame->width;

	auto pDestUData = pDestYData + destYLen;
	auto destULen = destYLen / 4;
	auto destUStride = desyStride / 2;

	auto pDestVData = pDestUData + destULen;

	if (0 != libyuv::I420Scale(pYData, yStride,
		pUData, uStride,
		pVData, vStride,
		bitmapHeader_.biWidth, bitmapHeader_.biHeight,
		pDestYData, desyStride,
		pDestUData, destUStride,
		pDestVData, destUStride,
		pFrame->width, pFrame->height,
		libyuv::kFilterBilinear
	))
	{
		LOG() << "I420Scale error " << res;
		m_frameQueue->PushEmptyFrame(pFrame);
		return ;
	}

	m_frameQueue->PushUsedFrame(pFrame);
}

void WinCapture::CaptureCursor(const RECT& winRect)
{
	POINT pos;
	if (!GetCursorPos(&pos))
	{
		return;
	}
	
	LONG dx = pos.x - winRect.left;
	LONG dy = pos.y - winRect.top;
	if (dx <= 0 || dy <= 0 || dx >= (winRect.right - winRect.left) || dy >= (winRect.bottom - winRect.top))
	{
		return;
	}

	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	if (!GetCursorInfo(&info) || info.flags != CURSOR_SHOWING)
	{
		return;
	}

	DrawIcon(m_hMemDC, dx, dy, info.hCursor);
}

int WinCapture::CpatureFrame()
{
	LONG w(0), h(0);
	RECT rect = { 0 };
	BOOL bUseBlankOrLastCaptureImage = false;

	if (m_hWnd == NULL)
	{
		w = m_iFrameWidth;
		h = m_iFrameHeight;
		bUseBlankOrLastCaptureImage = true;
	}
	else
	{
		if (IsIconic(m_hWnd) || !IsWindow(m_hWnd))
		{
			w = m_oldWidth;
			h = m_oldHeight;
			bUseBlankOrLastCaptureImage = true;
		}
		else
		{
			//GetClientRect(m_hWnd, &rect);
			GetWindowRect(m_hWnd, &rect);
			w = rect.right - rect.left;
			h = rect.bottom - rect.top;
		}
	}

	if (w % 2 != 0)
	{
		w += 1;
	}
	if (h % 2 != 0)
	{
		h += 1;
	}

	if (w != m_oldWidth || h != m_oldHeight)
	{
		if (m_hBitmap)
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}

		CreateBitmap(w, h);
		try
		{
			m_pYUVData.reset(new unsigned char[w * h * 3/2]);
			pBmpDataUpSideDown.reset(new unsigned char[w * h * 4]);
		}
		catch (...)
		{
			LOG() << "bad alloc";
			return CodeInternalError;
		}

		m_rectOLD = rect;
		m_oldWidth = w;
		m_oldHeight = h;
	}

	if (!m_hBitmap)
	{
		LOG() << "failed to create HBITMAP";
		return CodeInternalError;
	}

	if (bUseBlankOrLastCaptureImage)
	{
		SaveYUV(); // 直接用空白图像
		return CodeOK;
	}

	BOOL res(FALSE);
	if (m_iWindowType == WindowType::DesktopWindow)
		//res = BitBlt(m_hMemDC, 0, 0, w, h, m_hDC, 0, 0, SRCCOPY | CAPTUREBLT);
		res = BitBlt(m_hMemDC, 0, 0, w, h, m_hDC, 0, 0, SRCCOPY);
	else
		res = PrintWindow(m_hWnd, m_hMemDC, m_printWindowFlag);
	if (!res)
	{
		auto err = GetLastError();
		return err;
	}
	else
	{
		//SaveBitmap();
		//CaptureCursor(rect);
		SaveYUV();
	}

	return CodeOK;
}

int WinCapture::SwitchWindow(HWND hwnd)
{
	LOG() << "Switch window!!!";

	m_hWnd = hwnd;
	m_oldHeight = 0;
	m_oldWidth = 0;

	return CodeOK;
}
