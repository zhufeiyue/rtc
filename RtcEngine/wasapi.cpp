#include "wasapi.h"
#include <functiondiscoverykeys_devpkey.h>
#include "../common/common.h"

#define BUFFER_TIME_100NS (2 * 10000000)

namespace Wasapi
{
	void GetWASAPIAudioDevices_(std::vector<AudioDeviceInfo>& devices, bool input)
	{
		IMMDeviceEnumerator* enumerator = nullptr;
		IMMDeviceCollection* collection = nullptr;
		UINT count = 0;
		HRESULT res = S_FALSE;

		res = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, 
			__uuidof(IMMDeviceEnumerator), (void**)&enumerator);
		if (FAILED(res))
		{
			LOG() << "CoCreateInstance MMDeviceEnumerator failed";
			goto End;
		}

		res = enumerator->EnumAudioEndpoints(input ? eCapture : eRender, 
			DEVICE_STATE_ACTIVE, &collection);
		if (FAILED(res))
		{
			LOG() << "EnumAudioEndpoints failed";
			goto End;
		}

		res = collection->GetCount(&count);
		if (FAILED(res))
		{
			LOG() << "IMMDeviceCollection GetCount failed";
			goto End;
		}

		for (UINT i = 0; i < count; i++) 
		{
			IMMDevice* pDevice = nullptr;
			wchar_t* pDeviceID = nullptr;
			IPropertyStore* pProperty = nullptr;
			size_t len = 0;
			AudioDeviceInfo info;

			res = collection->Item(i, &pDevice);
			if (SUCCEEDED(res))
			{
				res = pDevice->GetId(&pDeviceID);
				if (SUCCEEDED(res) && pDeviceID && *pDeviceID)
				{
					len = wcslen(pDeviceID);
					info.id = std::wstring(pDeviceID, len);
					CoTaskMemFree(pDeviceID);
					pDeviceID = nullptr;
				}

				res = pDevice->OpenPropertyStore(STGM_READ, &pProperty);
				if (SUCCEEDED(res))
				{
					PROPVARIANT nameVar;
					PropVariantInit(&nameVar);

					res = pProperty->GetValue(PKEY_Device_FriendlyName, &nameVar);
					if (SUCCEEDED(res) && nameVar.pwszVal && *nameVar.pwszVal)
					{
						len = wcslen(nameVar.pwszVal);
						info.name = std::wstring(nameVar.pwszVal, len);
					}

					PropVariantClear(&nameVar);
					pProperty->Release();
					pProperty = nullptr;

				}

				pDevice->Release();
				pDevice = nullptr;

				devices.push_back(info);
			}
		}

	End:
		if (collection)
			collection->Release();
		if (enumerator)
			enumerator->Release();
	}

	void GetWASAPIAudioDevices(std::vector<AudioDeviceInfo>& devices, bool input)
	{
		devices.clear();

		try 
		{
			GetWASAPIAudioDevices_(devices, input);
		}
		catch (...) 
		{
		}
	}

	WASAPICapture::WASAPICapture(CaptureType type):m_captureType(type)
	{
	}

	WASAPICapture::~WASAPICapture()
	{
	}

	int WASAPICapture::Init()
	{
		IMMDeviceEnumerator* pEnum = nullptr;
		HRESULT hr = S_FALSE;

		if (m_threadWork.joinable())
		{
			return CodeFalse;
		}

		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator), (void**)&pEnum);
		if (!SUCCEEDED(hr))
		{
			return CodeFalse;
		}

		// get IMMDevice interface
		if (m_strDeviceId == L"default")
		{
			if (m_captureType == CaptureType::Loopback ||
				m_captureType == CaptureType::Render)
				hr = pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &m_pDevice);
			else if (m_captureType == CaptureType::Input)
				hr = pEnum->GetDefaultAudioEndpoint(eCapture, eCommunications, &m_pDevice);
			else
				hr = E_INVALIDARG;
		}
		else
		{
			hr = pEnum->GetDevice(m_strDeviceId.c_str(), &m_pDevice);
		}

		pEnum->Release();
		pEnum = nullptr;
		
		if (SUCCEEDED(hr))
		{
			IPropertyStore* pStore = nullptr;
			PWAVEFORMATEX deviceFormatProperties;
			PROPVARIANT prop;

			hr = m_pDevice->OpenPropertyStore(STGM_READ, &pStore);
			if (SUCCEEDED(hr))
			{
				hr = pStore->GetValue(PKEY_AudioEngine_DeviceFormat, &prop);
				if (SUCCEEDED(hr) && prop.vt != VT_EMPTY && prop.blob.pBlobData)
				{
					deviceFormatProperties = (PWAVEFORMATEX)prop.blob.pBlobData;
					m_devcieSample = deviceFormatProperties->nSamplesPerSec;
				}

				pStore->Release();
				pStore = nullptr;
			}
		}

		// get IAudioClient interface
		if (m_pDevice)
		{
			hr = m_pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pClient);
		}
		else
		{
			return CodeFalse;
		}

		if (CodeOK == InitClient() && CodeOK == InitService())
		{
			m_bWorking = true;
			m_threadWork = std::thread([this]()
				{
					WorkLoop();
				});

			hr = m_pClient->Start();
		}
		else
		{
			return CodeFalse;
		}

		return CodeOK;
	}

	int WASAPICapture::Destroy()
	{
		m_bWorking = false;
		if (m_threadWork.joinable())
			m_threadWork.join();

		if (m_pDevice)
		{
			m_pDevice->Release();
			m_pDevice = nullptr;
		}
		if (m_pClient)
		{
			m_pClient->Release();
			m_pClient = nullptr;
		}
		if (m_pClientCapture)
		{
			m_pClientCapture->Release();
			m_pClientCapture = nullptr;
		}
		if (m_pClientRender)
		{
			m_pClientRender->Release();
			m_pClientRender = nullptr;
		}

		return CodeOK;
	}

	int WASAPICapture::InitClient()
	{
		HRESULT hr = S_FALSE;
		WAVEFORMATEX* pWFX = nullptr;

		if (!m_pClient)
		{
			return CodeFalse;
		}

		hr = m_pClient->GetMixFormat(&pWFX);
		if (SUCCEEDED(hr))
		{
			m_format.Format = *pWFX;
			if (pWFX->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
			{
				WAVEFORMATEXTENSIBLE* ext = (WAVEFORMATEXTENSIBLE*)pWFX;

				/* WASAPI is always float */
				ext->SubFormat;  KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; WAVE_FORMAT_IEEE_FLOAT;

				m_format.Samples = ext->Samples;
				m_format.dwChannelMask = ext->dwChannelMask;
				m_format.SubFormat = ext->SubFormat;
			}
			else
			{
				LOG() << "IAudioClient::GetMixFormat return not WAVE_FORMAT_EXTENSIBLE";
				WAVE_FORMAT_PCM;
				WAVE_FORMAT_IEEE_FLOAT;
				WAVE_FORMAT_EXTENSIBLE;
			}

			UINT32 flag = 0;
			//flag |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK; // mush call IAudioClientg::SetEventHandle
			if (m_captureType == CaptureType::Loopback)
				flag |= AUDCLNT_STREAMFLAGS_LOOPBACK; // loopback capture;

			hr = m_pClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
				flag,
				BUFFER_TIME_100NS, 0, pWFX, nullptr);
			CoTaskMemFree(pWFX);
			if (FAILED(hr))
			{
				LOG() << "IAudioClient::Initialize 0x" << std::hex << hr;
				return CodeFalse;
			}
		}
		else
		{
			return CodeFalse;
		}

		return CodeOK;
	}

	int WASAPICapture::InitService()
	{
		HRESULT hr = S_FALSE;

		if (m_pClient)
		{
			// get IAudioCaptureClient interface
			hr = m_pClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pClientCapture);
			if (SUCCEEDED(hr) && m_pClientCapture)
			{
				return CodeOK;
			}
		}

		return CodeFalse;
	}

	int WASAPICapture::WorkLoop()
	{
		while (m_bWorking)
		{
			// 周期为10ms
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (CodeOK != ProcessData())
			{
				m_bWorking = false;
			}
		}

		if (m_pClient)
			m_pClient->Stop();

		return CodeOK;
	}

	int WASAPICapture::ProcessData()
	{
		if (!m_pClientCapture) 
		{
			return CodeFalse;
		}

		HRESULT res;
		LPBYTE buffer;
		UINT32 frames;
		DWORD flags;
		UINT64 pos, ts;
		UINT captureSize = 0;

		while (true)
		{
			// 周期为10ms 采样率48000的，每次能读到480帧
			res = m_pClientCapture->GetNextPacketSize(&captureSize);
			if (FAILED(res))
			{
				LOG() << "IAudioCaptureClient::GetNextPacketSize failed" << std::hex << "0x" << res;
				return CodeFalse;
			}
			if (!captureSize)
			{
				// silence
				return CodeOK;
			}

			res = m_pClientCapture->GetBuffer(&buffer, &frames, &flags, &pos, &ts);
			if (FAILED(res))
			{
				LOG() << "IAudioCaptureClient::GetBuffer failed " << std::hex << "0x" << res;
				return CodeFalse;
			}
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
			}

			//LOG() << frames << '-' << captureSize << '-' << pos << '-' << ts;
			if (m_audioCb)
			{
				m_audioCb(buffer, frames, pos, ts);
			}

			m_pClientCapture->ReleaseBuffer(frames);
		}

		return CodeOK;
	}


	int WASAPIPlay::InitClient()
	{
		HRESULT hr = S_FALSE;
		WAVEFORMATEX* pWFX = nullptr;

		if (!m_pClient)
		{
			return CodeFalse;
		}

		pWFX = (WAVEFORMATEX*)&m_format;
		hr = m_pClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 
			AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
			BUFFER_TIME_100NS, 0, pWFX, nullptr);
		if (FAILED(hr))
		{
			LOG() << "IAudioClient::Initialize 0x" << std::hex << hr;
			return CodeFalse;
		}

		return CodeOK;
	}

	int WASAPIPlay::InitService()
	{
		HRESULT hr = S_FALSE;

		if (m_pClient)
		{
			// get IAudioCaptureClient interface
			hr = m_pClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_pClientRender);
			if (SUCCEEDED(hr) && m_pClientRender)
			{
				return CodeOK;
			}
		}

		return CodeFalse;
	}

	int WASAPIPlay::ProcessData()
	{
		if (!m_pClientRender)
		{
			return CodeFalse;
		}
		UINT32 frames = m_format.Format.nSamplesPerSec / 100 * 2;
		LPBYTE pData = nullptr;
		UINT32 flag = 0;
		int n = 0;
		auto hr = m_pClientRender->GetBuffer(frames, &pData);
		if (SUCCEEDED(hr))
		{
			if (m_audioCb && (n = m_audioCb(pData, frames, 0, 0)) != 0)
			{
				frames = n; // 获得了多少帧的数据
			}
			else
			{
				flag |= AUDCLNT_BUFFERFLAGS_SILENT;
				memset(pData, 0, frames * m_format.Format.nBlockAlign);
			}
			hr = m_pClientRender->ReleaseBuffer(frames, flag);
		}
		else
		{
			LOG() << "IAudioRenderClient::GetBuffer 0x" << std::hex << hr;
			if (hr == AUDCLNT_E_BUFFER_TOO_LARGE)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}

		return CodeOK;
	}
}
