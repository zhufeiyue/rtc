#include "CaptureWindowWorker.h"
#include "X264Wrap.h"
#include "MFCodecWrap.h"
#include "WindowCapture.h"
#include "H264Encoder.h"
#include "FrameQueue.h"
#include "MFDevice.h"
#include "MFCapture.h"
#include <mfidl.h>
#include <future>
#include "libdshowcapture/dshowcapture.hpp"

void test()
{
	std::chrono::steady_clock::time_point tp1, tp2;
	auto vcc = CaptureConfigure();
	//vcc.width = 1920;
	//vcc.height = 1080;

	auto pCapture = new WindowCapture(GetDesktopWindow());
	pCapture->SetFrameSize(vcc.width, vcc.height);
	pCapture->SetWindowType(WindowCapture::WindowType::DesktopWindow);
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
	pWork->SetWindowType(WindowCapture::WindowType::DesktopWindow);
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
	//auto pH264Encoder = new H264Encoder<FrameQueue, MFH264Encoder>();
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
	pCapture->EndCapture();
	delete pCapture;
}

void test3()
{
	//auto pMFDevice = new MFDevice();
	//pMFDevice->EnumDevice(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

	std::vector<DShow::VideoDevice> devicesVideo;
	std::vector<DShow::AudioDevice> devicesAudio;
	DShow::Device::EnumVideoDevices(devicesVideo);
	//DShow::Device::EnumAudioDevices(devicesAudio);

	int frameRate = 25;
	FrameQueue* fq = new FrameQueue();
	//auto pH264 = new X264Wrap();
	auto pH264 = new MFH264Encoder();
	auto pH264Encoder = new H264Encoder<FrameQueue, MFH264Encoder>();

	DShow::VideoConfig vConfig;
	vConfig.cx = 1024;
	vConfig.cy_abs = 576;
	vConfig.format = vConfig.internalFormat = DShow::VideoFormat::I420;
	vConfig.frameInterval = 10000000 / frameRate;
	vConfig.name = devicesVideo[1].name;
	//static_cast<DShow::Config&>(vConfig).useDefaultConfig = false;
	vConfig.callback = [&fq](const DShow::VideoConfig& config, unsigned char* data,
		size_t size, long long startTime, long long stopTime, long rotation) 
	{
		if (config.format != DShow::VideoFormat::I420)
		{
			return;
		}

		auto pFrame = fq->PopEmptyFrame(config.cx, config.cy_abs, size);
		if (pFrame)
		{
			memcpy(pFrame->pData, data, size);
			fq->PushUsedFrame(pFrame);
		}
	};

	auto pDevice = new DShow::Device();
	pDevice->ResetGraph();
	pDevice->SetVideoConfig(&vConfig);
	pDevice->ConnectFilters();
	pDevice->GetVideoConfig(vConfig);
	frameRate = 1000000000 / (vConfig.frameInterval * 100 );
	
	fq->SetFrameSize(vConfig.cx, vConfig.cy_abs);
	auto ec = EncoderConfig();
	ec.width = vConfig.cx;
	ec.height = vConfig.cy_abs;
	ec.frameRate = frameRate;

	pH264->SetFrameSize(ec.width, ec.height, ec.frameRate);
	pH264->Init();
	pH264Encoder->SetEncoderConfig(ec);
	pH264Encoder->SetFrameQueue(fq);
	pH264Encoder->SetEncoder(pH264);
	if (CodeOK != pH264Encoder->Init())
	{
		return;
	}

	pDevice->Start();
	std::this_thread::sleep_for(std::chrono::seconds(30));
	pDevice->Stop();
}

int __stdcall WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	InitMFEnv();

	//test();
	//test1();
	//test2();
	test3();

	//auto pCapture = new VoiceCapture();
	//pCapture->StartCapture();
	//pCapture->EndCapture();
	//delete pCapture;

	std::this_thread::sleep_for(std::chrono::seconds(3));
	DestroyMFEnv();
	return 1;
}