#include "MFDevice.h"
#include <strsafe.h>
#include <mfapi.h>
#include <Mferror.h>

#include <mfapi.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Propsys.lib")

int InitMFEnv()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		hr = MFStartup(MF_VERSION);
		if (SUCCEEDED(hr))
		{
			return CodeOK;
		}
	}
	return CodeFalse;
}

int DestroyMFEnv()
{
	MFShutdown();
	CoUninitialize();
	return CodeOK;
}

HRESULT LogMediaType(IMFMediaType* pType);

MFDevice::MFDevice()
{
}

MFDevice::~MFDevice()
{
	if (m_ppDevices)
	{
		for (UINT32 i = 0; i < m_iDeviceCount; ++i)
		{
			m_ppDevices[i]->Release();
		}

		CoTaskMemFree(m_ppDevices);
	}

	if (m_pAttributes)
	{
		m_pAttributes->Release();
	}
}

HRESULT MFDevice::EnumDevice(IID type)
{
	HRESULT hr = S_FALSE;
	if (m_pAttributes || m_ppDevices)
	{
		return hr;
	}

	m_guidDeviceType = type;
	hr = MFCreateAttributes(&m_pAttributes, 2);
	if (FAILED(hr))
	{
		goto Done;
	}
	hr = m_pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, type);
	if (FAILED(hr))
	{
		goto Done;
	}
	m_iDeviceCount = 0;
	hr = MFEnumDeviceSources(m_pAttributes, &m_ppDevices, &m_iDeviceCount);
	if (FAILED(hr))
	{
		LOG() << "MFEnumDeviceSources fail " << std::hex << hr << std::dec;
		goto Done;
	}
	else
	{
		LOG() << "device num is " << m_iDeviceCount;
	}

	m_vDevicesInfo.clear();
	for (UINT32 i = 0; i < m_iDeviceCount; ++i)
	{
		GetDeviceInfo(m_ppDevices[i]);
	}

Done:
	return hr;
}

HRESULT MFDevice::GetDeviceInfo(IMFActivate* pActivate)
{
	HRESULT hr = S_FALSE;
	UINT32 cchName(0);
	WCHAR* szName = NULL;
	DeviceInfo info;

	if (!pActivate)
	{
		return hr;
	}
	
	hr = pActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &szName, &cchName);
	if (SUCCEEDED(hr))
	{
		info.m_strFriendlyName = std::wstring(szName, cchName);
		CoTaskMemFree(szName);
		szName = NULL;
	}

	if (m_guidDeviceType == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID)
	{
		//hr = pActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK, &szName, &cchName);
		hr = pActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID, &szName, &cchName);
	}
	else if (m_guidDeviceType == MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID)
	{
		hr = pActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &szName, &cchName);
	}
	else
	{
		hr = E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		info.m_strSymbolicLink = std::wstring(szName, cchName);
		CoTaskMemFree(szName);
		szName = NULL;
	}

	IMFMediaSource* pSource = NULL;
	hr = pActivate->ActivateObject(__uuidof(IMFMediaSource), (void**)&pSource);
	if (SUCCEEDED(hr))
	{
		IMFPresentationDescriptor* pPD = NULL;
		IMFStreamDescriptor* pSD = NULL;
		DWORD streamCount(0);
		BOOL bSelect = FALSE;

		hr = pSource->CreatePresentationDescriptor(&pPD);
		if (SUCCEEDED(hr))
		{
			hr = pPD->GetStreamDescriptorCount(&streamCount);
			for (DWORD i = 0; i < streamCount; ++i)
			{
				 hr = pPD->GetStreamDescriptorByIndex(i, &bSelect, &pSD);
				 if (SUCCEEDED(hr))
				 {
					 StreamInfo sinfo;
					 GetDeviceStreamInfo(pSD, sinfo);
					 pSD->Release();
					 info.m_vStreamInfo.push_back(std::move(sinfo));
				 }
			}
			pPD->Release();
		}

		pSource->Release();
	}

	m_vDevicesInfo.push_back(std::move(info));

	return hr;
}

HRESULT MFDevice::GetDeviceStreamInfo(IMFStreamDescriptor* pSD, StreamInfo& sinfo)
{
	HRESULT hr = S_FALSE;
	IMFMediaTypeHandler* pHandler = NULL;
	IMFMediaType* pType = NULL;
	DWORD cTypes = 0;

	if (!pSD)
	{
		return hr;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = pHandler->GetMediaTypeCount(&cTypes);
	if (FAILED(hr))
	{
		pHandler->Release();
		return hr;
	}

	for (DWORD i = 0; i < cTypes; i++)
	{
		hr = pHandler->GetMediaTypeByIndex(i, &pType);
		if (SUCCEEDED(hr))
		{
			MediaInfo mInfo;
			GetDeviceStreamMediaInfo(pType, mInfo);
			sinfo.m_vMediaInfo.push_back(std::move(mInfo));

			pType->Release();
			pType = NULL;
		}
		else
		{
			std::cout << "GetMediaTypeByIndex error" << std::endl;
			break;
		}
	}
	pHandler->Release();

	return hr;
}

HRESULT MFDevice::GetDeviceStreamMediaInfo(IMFMediaType* pType, MediaInfo& mInfo)
{
	HRESULT hr = S_FALSE;
	UINT32 count = 0;
	GUID guid = { 0 };
	PROPVARIANT var;
	UINT32 low, high;

	if (!pType)
	{
		return hr;
	}
	pType->IsCompressedFormat(&mInfo.m_bIsCompress);

	hr = pType->GetCount(&count);
	if (FAILED(hr))
	{
		return hr;
	}

	for (UINT32 i = 0; i < count; i++)
	{
		PropVariantInit(&var);

		hr = pType->GetItemByIndex(i, &guid, &var);
		if (FAILED(hr))
		{
			break;
		}

		if (guid == MF_MT_FRAME_SIZE)
		{
			Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &mInfo.m_frameWidth, &mInfo.m_frameHeight);
		}
		else if (guid == MF_MT_FRAME_RATE)
		{
			Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
			mInfo.m_frameRate = 1.0f * high / low;
		}
		else if (guid == MF_MT_FRAME_RATE_RANGE_MIN)
		{
			Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
			mInfo.m_minFrameRate = 1.0f * high / low;
		}
		else if (guid == MF_MT_FRAME_RATE_RANGE_MAX)
		{
			Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
			mInfo.m_maxFrameRate = 1.0f * high / low;
		}
		else if (guid == MF_MT_SUBTYPE)
		{
			mInfo.m_subType = *var.puuid;
			MFAudioFormat_Float;
		}
		else if (guid == MF_MT_MAJOR_TYPE)
		{
			mInfo.m_majorType = *var.puuid;
			// MFMediaType_Video 
			// MFMediaType_Audio
		}
		else if (guid == MF_MT_AUDIO_SAMPLES_PER_SECOND)
		{
			mInfo.m_audioSampleRate = var.ulVal;
		}
		else if (guid == MF_MT_AUDIO_NUM_CHANNELS)
		{
			mInfo.m_audioChannelNum = var.ulVal;
		}
		else if (guid == MF_MT_AUDIO_BITS_PER_SAMPLE)
		{
			mInfo.m_audioSampleBits = var.ulVal;
		}
		else if (guid == MF_MT_AUDIO_BLOCK_ALIGNMENT)
		{
			mInfo.m_audioBlockAlign = var.ulVal;
		}

		PropVariantClear(&var);
	}

	return hr;
}

IMFActivate* MFDevice::GetDeviceActivate(size_t index)
{
	if (!m_ppDevices || index >= m_iDeviceCount)
	{
		return NULL;
	}

	return m_ppDevices[index];
}

int MFDevice::ChooseVideo(DesiredVideoConfig conf, size_t whichDeviceIndex, size_t& streamIndex, size_t& mediaIndex)
{
	if (!m_ppDevices)
	{
		return CodeFalse;
	}

	if (m_guidDeviceType != MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID)
	{
		return CodeFalse;
	}

	if (whichDeviceIndex >= m_vDevicesInfo.size())
	{
		return CodeFalse;
	}

	streamIndex = 0;
	mediaIndex = 0;

	struct Score
	{
		size_t streamIndex = 0;
		size_t mediaIndex = 0;
		float score = 0;
#ifdef _DEBUG
		UINT32 w = 0, h = 0;
		float fr = 0;
#endif
	};

	std::vector<Score> vScores;
	const float totalSize = 120;
	const float totalFR = 60;
	const float totalFormat = 60;
	float weightSize;
	float weightFR;
	float weightFormat;
	float m;
	float desiredRatio = 1.0f * conf.width / conf.height;

	for (size_t i = 0; i < m_vDevicesInfo[whichDeviceIndex].m_vStreamInfo.size(); ++i)
	{
		auto& sinfo = m_vDevicesInfo[whichDeviceIndex].m_vStreamInfo[i];
		for (size_t j = 0; j < sinfo.m_vMediaInfo.size(); ++j)
		{
			auto& minfo = sinfo.m_vMediaInfo[j];
			Score s;
			s.streamIndex = i;
			s.mediaIndex = j;
#ifdef _DEBUG
			s.w = minfo.m_frameWidth;
			s.h = minfo.m_frameHeight;
			s.fr = minfo.m_frameRate;
#endif

			weightSize = 0;
			weightFR = 0.3f;
			weightFormat = 0;

			if (minfo.m_frameWidth == 0 || minfo.m_frameHeight == 0 || minfo.m_frameRate <= 0.01f)
			{
				s.score = 0;
				vScores.push_back(s);
				continue;
			}

			if (minfo.m_frameWidth == conf.width)
				weightSize += 0.5f;
			if (minfo.m_frameHeight == conf.height)
				weightSize += 0.5f;
			m = 1.0f * minfo.m_frameWidth / minfo.m_frameHeight;
			m = std::abs(m - desiredRatio);
			weightSize = weightSize + 1.0f - m / desiredRatio;
			
			if (std::abs(minfo.m_frameRate - conf.frameRate) < 1.0f)
				weightFR = 1.0f;
			else
				weightFR = minfo.m_frameRate / 120.0f;

			if (IsEqualGUID(minfo.m_subType, MFVideoFormat_I420) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_YUY2) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_YV12) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_RGB24) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_RGB32) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_RGB8) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_RGB555) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_AYUV) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_UYVY) ||
				IsEqualGUID(minfo.m_subType, MFVideoFormat_IYUV)
				)
			{
				weightFormat = 1.0f;
			}

			s.score = weightSize * totalSize + weightFR * totalFR + weightFormat * totalFormat;
			vScores.push_back(s);
		}
	}

	std::stable_sort(vScores.begin(), vScores.end(), [](const Score& l, const Score& r)
		{
			return l.score > r.score;
		});
	if (vScores.empty())
	{
		return CodeFalse;
	}

	streamIndex = vScores[0].streamIndex;
	mediaIndex = vScores[0].mediaIndex;

	return CodeOK;
}

int MFDevice::ChooseAudio(DesiredAudioConfig conf, size_t deviceIndex, size_t& streamIndex, size_t& mediaIndex)
{
	if (!m_ppDevices)
	{
		return CodeFalse;
	}

	if (m_guidDeviceType != MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID)
	{
		return CodeFalse;
	}

	if (deviceIndex >= m_vDevicesInfo.size())
	{
		return CodeFalse;
	}

	streamIndex = 0;
	mediaIndex = 0;

	return CodeOK;
}