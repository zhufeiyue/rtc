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
#include "wasapi.h"
#include "ColorspaceConvert.h"

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
	int frameFormat = 0;
	IConvertColorspace* pCSPConvert = nullptr;
	FrameQueue* fq = new FrameQueue();
	auto pH264 = new X264Wrap();
	auto pH264Encoder = new H264Encoder<FrameQueue, X264Wrap>();
	DShow::VideoConfig vConfig;

	vConfig.cx = 1024;
	vConfig.cy_abs = 576;
	vConfig.format = vConfig.internalFormat = DShow::VideoFormat::XRGB;
	vConfig.frameInterval = 10000000 / frameRate;
	vConfig.name = devicesVideo[0].name;
	//static_cast<DShow::Config&>(vConfig).useDefaultConfig = false;
	vConfig.callback = [&fq, &pCSPConvert](const DShow::VideoConfig& config, unsigned char* data,
		size_t size, long long startTime, long long stopTime, long rotation) 
	{
		if (pCSPConvert)
		{
			if (CodeOK != pCSPConvert->ProcessInput(data, size))
			{
				return;
			}

			data = pCSPConvert->GetOutputData();
			size = pCSPConvert->GetOutputSize();

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

	switch (vConfig.internalFormat)
	{
	case DShow::VideoFormat::I420:
		frameFormat = X264_CSP_I420;
		break;
	case DShow::VideoFormat::YV12:
		frameFormat = X264_CSP_YV12;
		break;
	case DShow::VideoFormat::NV12:
		frameFormat = X264_CSP_NV12;
		frameFormat = X264_CSP_I420;
		pCSPConvert = new Convert2I420< libyuv::FOURCC_NV12>(vConfig.cx, vConfig.cy_abs);
		break;
	case DShow::VideoFormat::Y800:
		frameFormat = X264_CSP_I400;
		frameFormat = X264_CSP_I420;
		pCSPConvert = new Convert2I420< libyuv::FOURCC_I400>(vConfig.cx, vConfig.cy_abs);
		break;
	case DShow::VideoFormat::YUY2: 
		frameFormat = X264_CSP_YUYV;
		frameFormat = X264_CSP_I420;
		pCSPConvert = new Convert2I420< libyuv::FOURCC_YUY2>(vConfig.cx, vConfig.cy_abs);
		break;
	case DShow::VideoFormat::UYVY:
	case DShow::VideoFormat::HDYC: // BT709
		frameFormat = X264_CSP_UYVY;
		frameFormat = X264_CSP_I420;
		pCSPConvert = new Convert2I420< libyuv::FOURCC_UYVY>(vConfig.cx, vConfig.cy_abs);
		break;
	default:
		return;
	}
	if (vConfig.cy_flip)
		frameFormat |= X264_CSP_VFLIP;

	pH264->SetFrameSize(ec.width, ec.height, ec.frameRate, frameFormat);
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

void test4()
{
	std::vector<Wasapi::AudioDeviceInfo> devices;
	Wasapi::GetWASAPIAudioDevices(devices, false);

	auto pWavRead = new WavDeMuxerFile("d:\\myworld.wav");
	auto pWavPlay = new Wasapi::WASAPIPlay();
	if (CodeOK == pWavRead->ReadFormat())
	{
		pWavPlay->SetAudioCb([pWavRead, pWavPlay](uint8_t* pData, uint32_t frames, uint64_t, uint64_t)
			{
				uint32_t got;
				if (CodeOK != pWavRead->ReadSample(pData, 
					frames * pWavRead->AudioFormat().Format.nBlockAlign, 
					got))
				{
					return (int)CodeFalse;
				}
				else
				{
					return (int)(got / pWavRead->AudioFormat().Format.nBlockAlign);
				}
			});

		pWavPlay->SetAudioFormat(pWavRead->AudioFormat());
		pWavPlay->Init();
		std::this_thread::sleep_for(std::chrono::seconds(600));
		pWavPlay->Destroy();
	}
	return;

	WavMuxerFile* pSaveWav = nullptr;
	auto p = new Wasapi::WASAPILoopbackCapture();
	//auto p = new Wasapi::WASAPICapture(Wasapi::WASAPICapture::CaptureType::Input);
	p->SetAudioCb([&pSaveWav, p](uint8_t* pData, uint32_t frames, uint64_t pos, uint64_t ts)
		{
			pSaveWav->WriteSample(frames, pData, frames * p->AudioFormat().Format.nBlockAlign);
			return CodeOK;
		});
	if (CodeOK == p->Init())
	{
		pSaveWav = new WavMuxerFile(p->AudioFormat(), "d:\\record.wav");
		pSaveWav->WriteRIFF(0);
		pSaveWav->Writefmt(0);
		pSaveWav->Writefact(0);
		pSaveWav->Writedata(0);

		std::this_thread::sleep_for(std::chrono::seconds(30));
		p->Destroy();
		delete p;

		pSaveWav->Flush();
		delete pSaveWav;
	}
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
	//test3();
	test4();

	//auto pCapture = new VoiceCapture();
	//pCapture->StartCapture();
	//pCapture->EndCapture();
	//delete pCapture;

	std::this_thread::sleep_for(std::chrono::seconds(3));
	DestroyMFEnv();
	return 1;
}