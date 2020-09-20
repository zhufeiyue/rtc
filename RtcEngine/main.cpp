#include <Windows.h>
#include "CaptureWindowWorker.h"
#include "X264Wrap.h"
#include "MFCodecWrap.h"
#include "WinCapture.h"
#include "H264Encoder.h"
#include "FrameQueue.h"
#include "MFDevice.h"
#include "MFCapture.h"
#include <mfidl.h>

void test()
{
	std::chrono::steady_clock::time_point tp1, tp2;
	auto vcc = CaptureConfigure();
	//vcc.width = 1920;
	//vcc.height = 1080;

	auto pCapture = new WinCapture(GetDesktopWindow());
	pCapture->SetFrameSize(vcc.width, vcc.height);
	pCapture->SetWindowType(WinCapture::WindowType::DesktopWindow);
	pCapture->Init();

	//auto pX264 = new X264Wrap();
	auto pX264 = new MFH264Encoder();
	pX264->SetFrameSize(vcc.width, vcc.height, vcc.frameRate);
	if (CodeOK == pX264->Init())
	{
		uint8_t* yuv[3];
		auto size = vcc.width * vcc.height;

		for (int i = 0; i < 550; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			tp1 = std::chrono::steady_clock::now();
			if (CodeOK == pCapture->CpatureFrame())
			{
				tp2 = std::chrono::steady_clock::now();
				LOG() << "capture time " << std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();

				auto p = pCapture->GetFrameQueue()->PopUsedFrame();
				if (p)
				{
					yuv[0] = p->pData;
					yuv[1] = p->pData + size;
					yuv[2] = p->pData + size / 4 + size;

					tp1 = std::chrono::steady_clock::now();
					pX264->ProcessInput(yuv, 0);
					tp2 = std::chrono::steady_clock::now();
					LOG() << "encode time " << std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();

					pCapture->GetFrameQueue()->PushEmptyFrame(p);
				}
			}
			else
			{
				LOG() << "capture error";
			}
		}
	}
}

void test1()
{
	auto pWork = new CaptureWindowWorker();
	auto vcc = CaptureConfigure();
	//vcc.width = 1024;
	//vcc.height = 576;
	pWork->SetHWND(GetDesktopWindow());
	pWork->SetVideoCaptureConfigure(vcc);
	pWork->SetWindowType(WinCapture::WindowType::DesktopWindow);
	if (CodeOK != pWork->Init())
	{
		return;
	}

	auto pX264 = new X264Wrap();
	//auto pX264 = new MFH264Encoder();
	pX264->SetFrameSize(vcc.width, vcc.height, vcc.frameRate);
	if (CodeOK != pX264->Init())
	{
		return;
	}

	auto ec = EncoderConfig();
	ec.width = vcc.width;
	ec.height = vcc.height;
	ec.frameRate = vcc.frameRate;
	ec.frameFormat = vcc.frameFormat;
	auto pH264Encoder = new H264Encoder<FrameQueue, X264Wrap>();
	pH264Encoder->SetEncoderConfig(ec);
	pH264Encoder->SetFrameQueue(pWork->GetFrameQueue());
	pH264Encoder->SetEncoder(pX264);
	if (CodeOK != pH264Encoder->Init())
	{
		return;
	}

	std::this_thread::sleep_for(std::chrono::seconds(30));
	pH264Encoder->Destroy();
	pWork->Destroy();
	pX264->Destroy();
}

void test2()
{
	size_t streamIndex(0), mediaIndex(0);
	auto pDevice = new MFDevice();
	pDevice->EnumDevice(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
	pDevice->ChooseAudio(DesiredAudioConfig(), 0, streamIndex, mediaIndex);

	auto pCapture = new MFCaptureAudio();
	pCapture->StartCapture(pDevice->GetDeviceActivate(0), streamIndex, mediaIndex);

	std::this_thread::sleep_for(std::chrono::seconds(30));
}

int __stdcall WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	InitMFEnv();

	//test();
	//test1();
	test2();

	std::this_thread::sleep_for(std::chrono::seconds(3));
	DestroyMFEnv();
	return 1;
}