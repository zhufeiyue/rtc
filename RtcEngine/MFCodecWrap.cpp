#include "MFCodecWrap.h"
#include <strsafe.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mferror.h>
#include <Wmcodecdsp.h>
#include <codecapi.h>
#include <propvarutil.h>

void DBGMSG(PCWSTR format, ...)
{
	va_list args;
	va_start(args, format);

	WCHAR msg[MAX_PATH];

	if (SUCCEEDED(StringCbVPrintf(msg, sizeof(msg), format, args)))
	{
		OutputDebugString(msg);
	}

	va_end(args);
}

#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return L#val
#endif
LPCWSTR GetGUIDNameConst(const GUID& guid)
{
	IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
	IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
	IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
	IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
	IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
	IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
	IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
	IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
	IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
	IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
	IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
	IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
	IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
	IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
	IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
	IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
	IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
	IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
	IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
	IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
	IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
	IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
	IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
	IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
	IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
	IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
	IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
	IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
	IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
	IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
	IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
	IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
	IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

	// Media types

	IF_EQUAL_RETURN(guid, MFMediaType_Audio);
	IF_EQUAL_RETURN(guid, MFMediaType_Video);
	IF_EQUAL_RETURN(guid, MFMediaType_Protected);
	IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
	IF_EQUAL_RETURN(guid, MFMediaType_Script);
	IF_EQUAL_RETURN(guid, MFMediaType_Image);
	IF_EQUAL_RETURN(guid, MFMediaType_HTML);
	IF_EQUAL_RETURN(guid, MFMediaType_Binary);
	IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

	IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
	IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
	IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
	IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
	IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
	IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
	IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
	IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
	IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
	IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
	IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
	IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
	IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
	IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

	IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
	IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
	IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
	IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
	IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
	IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
	IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
	IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
	IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
	IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

	return NULL;
}

HRESULT GetGUIDName(const GUID& guid, WCHAR** ppwsz)
{
	HRESULT hr = S_OK;
	WCHAR* pName = NULL;

	LPCWSTR pcwsz = GetGUIDNameConst(guid);
	if (pcwsz)
	{
		size_t cchLength = 0;

		hr = StringCchLength(pcwsz, STRSAFE_MAX_CCH, &cchLength);
		if (FAILED(hr))
		{
			goto done;
		}

		pName = (WCHAR*)CoTaskMemAlloc((cchLength + 1) * sizeof(WCHAR));

		if (pName == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto done;
		}

		hr = StringCchCopy(pName, cchLength + 1, pcwsz);
		if (FAILED(hr))
		{
			goto done;
		}
	}
	else
	{
		hr = StringFromCLSID(guid, &pName);
	}

done:
	if (FAILED(hr))
	{
		*ppwsz = NULL;
		if (pName)
			CoTaskMemFree(pName);
	}
	else
	{
		*ppwsz = pName;
	}
	return hr;
}

void LogUINT32AsUINT64(const PROPVARIANT& var)
{
	UINT32 uHigh = 0, uLow = 0;
	Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &uHigh, &uLow);
	DBGMSG(L"%d x %d", uHigh, uLow);
}

float OffsetToFloat(const MFOffset& offset)
{
	return offset.value + (static_cast<float>(offset.fract) / 65536.0f);
}

HRESULT LogVideoArea(const PROPVARIANT& var)
{
	if (var.caub.cElems < sizeof(MFVideoArea))
	{
		return MF_E_BUFFERTOOSMALL;
	}

	MFVideoArea* pArea = (MFVideoArea*)var.caub.pElems;

	DBGMSG(L"(%f,%f) (%d,%d)", OffsetToFloat(pArea->OffsetX), OffsetToFloat(pArea->OffsetY),
		pArea->Area.cx, pArea->Area.cy);
	return S_OK;
}

HRESULT SpecialCaseAttributeValue(GUID guid, const PROPVARIANT& var)
{
	if ((guid == MF_MT_FRAME_RATE) || (guid == MF_MT_FRAME_RATE_RANGE_MAX) ||
		(guid == MF_MT_FRAME_RATE_RANGE_MIN) || (guid == MF_MT_FRAME_SIZE) ||
		(guid == MF_MT_PIXEL_ASPECT_RATIO))
	{
		// Attributes that contain two packed 32-bit values.
		LogUINT32AsUINT64(var);
	}
	else if ((guid == MF_MT_GEOMETRIC_APERTURE) ||
		(guid == MF_MT_MINIMUM_DISPLAY_APERTURE) ||
		(guid == MF_MT_PAN_SCAN_APERTURE))
	{
		// Attributes that an MFVideoArea structure.
		return LogVideoArea(var);
	}
	else
	{
		return S_FALSE;
	}
	return S_OK;
}

HRESULT LogAttributeValueByIndex(IMFAttributes* pAttr, DWORD index)
{
	WCHAR* pGuidName = NULL;
	WCHAR* pGuidValName = NULL;

	GUID guid = { 0 };

	PROPVARIANT var;
	PropVariantInit(&var);

	HRESULT hr = pAttr->GetItemByIndex(index, &guid, &var);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = GetGUIDName(guid, &pGuidName);
	if (FAILED(hr))
	{
		goto done;
	}

	DBGMSG(L"\t%s\t", pGuidName);

	hr = SpecialCaseAttributeValue(guid, var);
	if (FAILED(hr))
	{
		goto done;
	}
	if (hr == S_FALSE)
	{
		switch (var.vt)
		{
		case VT_UI4:
			DBGMSG(L"%d", var.ulVal);
			break;

		case VT_UI8:
			DBGMSG(L"%I64d", var.uhVal);
			break;

		case VT_R8:
			DBGMSG(L"%f", var.dblVal);
			break;

		case VT_CLSID:
			hr = GetGUIDName(*var.puuid, &pGuidValName);
			if (SUCCEEDED(hr))
			{
				DBGMSG(pGuidValName);
			}
			break;

		case VT_LPWSTR:
			DBGMSG(var.pwszVal);
			break;

		case VT_VECTOR | VT_UI1:
			DBGMSG(L"<<byte array>>");
			break;

		case VT_UNKNOWN:
			DBGMSG(L"IUnknown");
			break;

		default:
			DBGMSG(L"Unexpected attribute type (vt = %d)", var.vt);
			break;
		}
	}

done:
	DBGMSG(L"\n");
	CoTaskMemFree(pGuidName);
	CoTaskMemFree(pGuidValName);
	PropVariantClear(&var);
	return hr;
}

HRESULT LogMediaType(IMFMediaType* pType)
{
	UINT32 count = 0;

	HRESULT hr = pType->GetCount(&count);
	if (FAILED(hr))
	{
		return hr;
	}

	if (count == 0)
	{
		DBGMSG(L"Empty media type.\n");
	}

	for (UINT32 i = 0; i < count; i++)
	{
		hr = LogAttributeValueByIndex(pType, i);
		if (FAILED(hr))
		{
			break;
		}
	}
	return hr;
}

MFH264Encoder::MFH264Encoder()
{
}

MFH264Encoder::~MFH264Encoder()
{
	if (m_pInputSample)
		m_pInputSample->Release();
	if (m_pEncoder)
		m_pEncoder->Release();
	if (m_dataOut.pSample)
		m_dataOut.pSample->Release();
}

void MFH264Encoder::SetFrameSize(int w, int h, int fr)
{
	if (w < 1 || w % 2 != 0 ||
		h < 1 || h % 2 != 0 ||
		fr < 1 || fr > 100)
	{
		LOG() << "MF h264 encoder cannt take configure width " << w << " height " << h << " frame rate " << fr;
		return;
	}

	m_iWidth = w;
	m_iHeight = h;
	m_iFrameRate = fr;
}

int MFH264Encoder::Init()
{
	HRESULT hr = S_FALSE;
	DWORD dwInID(0), dwOutID(0);

	if (m_pEncoder)
	{
		return CodeFalse;
	}
	if (m_iWidth == 0 || m_iHeight == 0 || m_iFrameRate == 0)
	{
		return CodeFalse;
	}

	hr = CoCreateInstance(CLSID_CMSH264EncoderMFT,
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_IMFTransform, (void**)&m_pEncoder);
	hr = m_pEncoder->GetStreamCount(&dwInID, &dwOutID);
	dwInID = 0;
	dwOutID = 0;
	hr = m_pEncoder->GetStreamIDs(1, &dwInID, 1, &dwOutID);
	MF_MT_MPEG_SEQUENCE_HEADER;

	IMFMediaType* pOutType = NULL;
	IMFMediaType* pInType = NULL;
	ICodecAPI* pCodec = NULL;
	VARIANT var;

	hr = m_pEncoder->QueryInterface(__uuidof(ICodecAPI), (void**)&pCodec);
	if (SUCCEEDED(hr))
	{
		InitVariantFromUInt32(eAVEncCommonRateControlMode_Quality, &var);
		hr = pCodec->SetValue(&CODECAPI_AVEncCommonRateControlMode, &var);
		InitVariantFromUInt32(50, &var);
		hr = pCodec->SetValue(&CODECAPI_AVEncCommonQuality, &var);
		InitVariantFromBoolean(TRUE, &var);
		hr = pCodec->SetValue(&CODECAPI_AVLowLatencyMode, &var);

		hr = S_OK;
	}

	hr = MFCreateMediaType(&pOutType);
	if (SUCCEEDED(hr))
	{
		hr = pOutType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		hr = pOutType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
		hr = pOutType->SetUINT32(MF_MT_AVG_BITRATE, 600 * 1000);
		hr = MFSetAttributeRatio(pOutType, MF_MT_FRAME_RATE, m_iFrameRate, 1);
		hr = MFSetAttributeSize(pOutType, MF_MT_FRAME_SIZE, m_iWidth, m_iHeight);
		hr = pOutType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
		//hr = pOutType->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_Normal);
		hr = pOutType->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_Base);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pEncoder->SetOutputType(dwOutID, pOutType, 0);
	}

	if (SUCCEEDED(hr))
	{
		UINT32 blobSize(0);
		UINT8 buf[512] = { 0 };
		pOutType->GetBlob(MF_MT_MPEG_SEQUENCE_HEADER, buf, sizeof(buf), &blobSize);
		m_strSequenceHeader = std::string((char*)buf, blobSize);

		DWORD index = 0;
		GUID subType;
		while (true)
		{
			hr = m_pEncoder->GetInputAvailableType(dwInID, index++, &pInType);
			if (FAILED(hr))
			{
				break;
			}

			pInType->GetGUID(MF_MT_SUBTYPE, &subType);
			if (subType == MFVideoFormat_YV12)
			{
				m_bYV12 = TRUE;
				break;
			}
			if (subType == MFVideoFormat_I420)
			{
				m_bYV12 = FALSE;
				break;
			}
			pInType->Release();
			pInType = NULL;
		}
	}
	if (!pInType)
	{
		LOG() << "h264 encoder transform:no input for MFVideoFormat_YV12 or MFVideoFormat_I420";
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pEncoder->SetInputType(dwInID, pInType, 0);
	}

	m_pEncoder->GetInputStreamInfo(dwInID, &m_streamInfoIn);
	if (SUCCEEDED(hr))
	{
		m_dwInStreamID = dwInID;
		m_dwOutStreamID = dwOutID;
		hr = m_pEncoder->GetOutputStreamInfo(dwOutID, &m_streamInfoOut);
		if (m_streamInfoOut.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES)
		{
			LOG() << "MFT_OUTPUT_STREAM_PROVIDES_SAMPLES h264 encoder";
			m_dataOut.pSample = NULL;
		}
		else
		{
			IMFSample* pSample = NULL;
			IMFMediaBuffer* pBuffer = NULL;

			hr = MFCreateSample(&pSample);
			if (SUCCEEDED(hr))
			{
				hr = MFCreateMemoryBuffer(m_streamInfoOut.cbSize, &pBuffer);
			}
			if (SUCCEEDED(hr))
			{
				hr = pSample->AddBuffer(pBuffer);
				pBuffer->Release();
				m_dataOut.dwStreamID = dwOutID;
				m_dataOut.pSample = pSample;
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		IMFSample* pSample = NULL;
		IMFMediaBuffer* pBuffer = NULL;

		hr = MFCreateSample(&pSample);
		if (SUCCEEDED(hr))
		{
			hr = MFCreateMemoryBuffer(m_iWidth * m_iHeight * 2, &pBuffer);
		}
		if (SUCCEEDED(hr))
		{
			hr = pSample->AddBuffer(pBuffer);
			pBuffer->Release();
			m_pInputSample = pSample;
		}
	}

	m_iInputSampleCount = 0;
	m_iSampleTime = 0;
	m_iSampleDuration = 1000000000 / m_iFrameRate / 100;
	if (m_pInputSample)
	{
		m_pInputSample->SetSampleDuration(m_iSampleDuration);
		m_pInputSample->SetSampleFlags(0);
	}

	if (pOutType)
		pOutType->Release();
	if (pInType)
		pInType->Release();
	if (pCodec)
		pCodec->Release();

	if (SUCCEEDED(hr))
		return CodeOK;
	else
		return CodeFalse;
}

int MFH264Encoder::Destroy()
{
	return CodeOK;
}

int MFH264Encoder::ProcessInput(uint8_t* yuv[], int)
{
	if (!m_pEncoder || !m_pInputSample)
	{
		return CodeFalse;
	}

	HRESULT hr;
	BYTE* buf = NULL;
	IMFMediaBuffer* pBuffer = NULL;
	int yLen = m_iHeight * m_iWidth;
	int uLen = yLen / 4;

	hr = m_pInputSample->GetBufferByIndex(0, &pBuffer);
	if (FAILED(hr))
	{
		return CodeFalse;
	}

	pBuffer->Lock(&buf, NULL, NULL);
	memcpy(buf, yuv[0], yLen);
	if (m_bYV12)
	{
		memcpy(buf + yLen, yuv[2], uLen);
		memcpy(buf + yLen + uLen, yuv[1], uLen);
	}
	else
	{
		memcpy(buf + yLen, yuv[1], uLen);
		memcpy(buf + yLen + uLen, yuv[2], uLen);
	}

	pBuffer->Unlock();
	pBuffer->SetCurrentLength(yLen + uLen + uLen);
	pBuffer->Release();
	pBuffer = NULL;

	DWORD dwStatus(0);
	DWORD dwSampleLen(0);
	m_pInputSample->SetSampleTime(++m_iInputSampleCount * m_iSampleDuration);

	hr = m_pEncoder->ProcessInput(m_dwInStreamID, m_pInputSample, 0);

	if (SUCCEEDED(hr))
	{
		hr = m_pEncoder->ProcessOutput(0, 1, &m_dataOut, &dwStatus);
		if (m_dataOut.pEvents)
		{
			m_dataOut.pEvents->Release();
			m_dataOut.pEvents = NULL;
		}
	}

	if (SUCCEEDED(hr))
	{
		static std::ofstream fileOut;
		if (!fileOut.is_open())
		{
			fileOut.open("d:\\2.h264", std::ofstream::binary | std::ofstream::out);
			//fileOut.write(m_strSequenceHeader.c_str(), m_strSequenceHeader.length());
		}
		if (fileOut.is_open())
		{
			m_dataOut.pSample->GetBufferByIndex(0, &pBuffer);
			pBuffer->Lock(&buf, NULL, &dwSampleLen);
			fileOut.write((const char*)buf, dwSampleLen);
			pBuffer->Unlock();
		}
	}

	if (pBuffer)
		pBuffer->Release();

	return CodeOK;
}


int ParseSampleRateByIndex(int iii)
{
	int res;
	switch (iii)
	{
	case 0:
		res = 96000;
		break;
	case 1:
		res = 88200;
		break;
	case 2:
		res = 64000;
		break;
	case 3:
		res = 48000;
		break;
	case 4:
		res = 44100;
		break;
	case 5:
		res = 32000;
		break;
	case 6:
		res = 24000;
		break;
	case 7:
		res = 22025;
		break;
	case 0x8:
		res = 16000;
		break;
	case 0x9:
		res = 12000;
		break;
	case 0xa:
		res = 11025;
		break;
	case 0xb:
		res = 8000;
		break;
	case 0xc:
		res = 7350;
		break;
	default:
		std::logic_error("unsupport samplerate");
		break;
	}

	return res;
}

void MFAACEncoder::ParseAudioSpecificConfig(AudioSpecificConfig& aac_head, const unsigned char* a, int len)
{
	unsigned char temp1, temp2, temp3;
	aac_head.aac_seq_head_audioObjectType = (a[0] & 0xf8) >> 3;
	aac_head.aac_seq_head_audioObjectType -= 1;
	aac_head.aac_seq_head_samplingFrequencyIndex = ((a[0] & 0x07) << 1) + ((a[1] & 0x80) >> 7);
	if (aac_head.aac_seq_head_samplingFrequencyIndex == 0xf)
	{
		temp1 = ((a[1] & 0x7f) << 1) | ((a[2] & 0x80) >> 7);
		temp2 = ((a[2] & 0x7f) << 1) | ((a[3] & 0x80) >> 7);
		temp3 = ((a[3] & 0x7f) << 1) | ((a[4] & 0x80) >> 7);
		aac_head.aac_seq_head_samplingFrequency = (temp1 << 16) | (temp2 << 8) | temp3;
		aac_head.aac_seq_head_channelConfiguration = (a[4] & 0x78) >> 3;
	}
	else
	{
		aac_head.aac_seq_head_channelConfiguration = (a[1] & 0x78) >> 3;
		aac_head.aac_seq_head_samplingFrequency = ParseSampleRateByIndex(aac_head.aac_seq_head_samplingFrequencyIndex);
	}
}

void MFAACEncoder::CreateAACADTSHeader(unsigned char* p, const AudioSpecificConfig& asc, int aacDataLen)
{
	memset(p, 0, 7); // adts header 7 bytes
	/*
	syncword			12  ; all bits 1
	ID					1   ; 0 mpeg4,1 mpeg2
	layer				2   ; all bits 0
	protection_absent	1   ; 1
	// 2 bytes
	profile             2   ;
	sampling_freq_inex  4
	private_bit         1
	channel_configura   3
	original_copy       1
	home                1

	copyright_bit       1
	copyright_start     1
	aac_frame_length    13  ; size ,include ADTS head size
	adts_buffer_fullness 11
	number_of_data_blocks 2
	*/

	unsigned int len = 7 + aacDataLen;
	p[0] = 0xff;
	p[1] = 0xf1;
	p[2] |= (asc.aac_seq_head_audioObjectType << 6);
	p[2] |= (asc.aac_seq_head_samplingFrequencyIndex << 2);
	p[2] |= (asc.aac_seq_head_channelConfiguration >> 2);
	p[3] |= (asc.aac_seq_head_channelConfiguration << 6);
	p[3] |= ((len & 0x1800) >> 11);
	p[4] = (len & 0x7f8) >> 3;
	p[5] = (len & 0x7) << 5;
	p[5] |= 0x1f;
	p[6] |= 0xfc;
}

MFAACEncoder::MFAACEncoder()
{
}

MFAACEncoder::~MFAACEncoder()
{
	if (m_dataOut.pSample)
	{
		m_dataOut.pSample->Release();
		m_dataOut.pSample = NULL;
	}
	if (m_pEncoder)
	{
		m_pEncoder->Release();
		m_pEncoder = NULL;
	}
}

void MFAACEncoder::SetInputSampleInfo(int fr, int channel, int bits)
{
	m_iInputSampleBits = bits;
	m_iInputSampleChannel = channel;
	m_iInputSampleRate = fr;
}

int MFAACEncoder::Init()
{
	HRESULT hr;
	DWORD dwInID(0), dwOutID(0);
	hr = CoCreateInstance(CLSID_AACMFTEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IMFTransform, (void**)&m_pEncoder);
	if (FAILED(hr))
	{
		return CodeFalse;
	}
	hr = m_pEncoder->GetStreamIDs(1, &dwInID, 1, &dwOutID); // 未实现此接口，H264 Encoder也一样。in stream id和out stream id 都是0

	IMFMediaType* pInputType(NULL), * pOutType(NULL);
	PROPVARIANT var;

	hr = MFCreateMediaType(&pInputType);
	hr = pInputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	hr = pInputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	hr = pInputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, (UINT32)m_iInputSampleBits);
	hr = pInputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, (UINT32)m_iInputSampleRate);
	hr = pInputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, (UINT32)m_iInputSampleChannel);
	hr = m_pEncoder->SetInputType(0, pInputType, 0);
	if (SUCCEEDED(hr))
	{
		hr = MFCreateMediaType(&pOutType);
		hr = pOutType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		hr = pOutType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
		hr = pOutType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, (UINT32)m_iInputSampleBits);
		hr = pOutType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, (UINT32)m_iInputSampleRate);
		hr = pOutType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, (UINT32)m_iInputSampleChannel);
		hr = pOutType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, (UINT32)m_iOutputBytePerSecond);
		hr = pOutType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 0);
		hr = pOutType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, (UINT32)0x29); // AAC Profile L2
		hr = m_pEncoder->SetOutputType(0, pOutType, 0);
	}

	if (SUCCEEDED(hr))
	{
		PropVariantInit(&var);
		pOutType->GetItem(MF_MT_USER_DATA, &var); // MF_MT_USER_DATA  vector of unsigned char  VT_VECTOR|VT_UI1
		if (SUCCEEDED(hr))
		{
			CAUB caub = var.caub; // 第13和14字节是AudioSpecificConfig
			auto a = caub.pElems + 12;
			ParseAudioSpecificConfig(m_aacAudioConfig, a, caub.cElems - 12);
			PropVariantClear(&var);
		}

		hr = m_pEncoder->GetInputStreamInfo(dwInID, &m_streamInfoIn);
		hr = m_pEncoder->GetOutputStreamInfo(dwOutID, &m_streamInfoOut);
	}

	if (SUCCEEDED(hr))
	{
		if (m_streamInfoOut.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES)
		{
			LOG() << "MFT_OUTPUT_STREAM_PROVIDES_SAMPLES aac encoder";
			m_dataOut.pSample = NULL;
		}

		IMFSample* pSample = NULL;
		IMFMediaBuffer* pBuffer = NULL;
		hr = MFCreateSample(&pSample);
		if (SUCCEEDED(hr))
		{
			hr = MFCreateMemoryBuffer(m_iInputSampleRate * m_iInputSampleChannel * m_iInputSampleBits / 8 / 2, &pBuffer);
		}
		if (SUCCEEDED(hr))
		{
			hr = pSample->AddBuffer(pBuffer);
			pBuffer->Release();
			m_dataOut.dwStreamID = dwOutID;
			m_dataOut.pSample = pSample;
		}
	}

	//hr = m_pEncoder->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
	//hr = m_pEncoder->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);

	if (pInputType)
		pInputType->Release();
	if (pOutType)
		pOutType->Release();

	if (SUCCEEDED(hr))
		return CodeOK;
	else
		return CodeFalse;
}

int MFAACEncoder::Destroy()
{
	return CodeOK;
}

int MFAACEncoder::ProcessInput(uint8_t* [], int)
{
	return CodeFalse;
}

int MFAACEncoder::ProcessInput(IMFSample* pSample)
{
	if (!pSample || !m_pEncoder)
	{
		return CodeFalse;
	}

	HRESULT hr = MF_E_TRANSFORM_NEED_MORE_INPUT;
	DWORD dwStatus(0);
	DWORD len(0);
	IMFMediaBuffer* pBuf = NULL;
	BYTE* pData = NULL;

	hr = m_pEncoder->ProcessInput(0, pSample, 0);
	if (SUCCEEDED(hr))
	{
	Again:
		hr = m_pEncoder->ProcessOutput(0, 1, &m_dataOut, &dwStatus);
		if (m_dataOut.pEvents)
		{
			m_dataOut.pEvents->Release();
			m_dataOut.pEvents = NULL;
		}
		if (SUCCEEDED(hr))
		{
			m_dataOut.pSample->GetTotalLength(&len); // one aac frame(1024 PCM sample)
			m_dataOut.pSample->ConvertToContiguousBuffer(&pBuf);
			pBuf->Lock(&pData, NULL, &len);
			{
				static std::ofstream fileOut;
				if (!fileOut.is_open())
				{
					fileOut.open("d:\\1.aac", std::ofstream::binary | std::ofstream::out);
				}
				unsigned char adtsheader[7];
				CreateAACADTSHeader(adtsheader, this->m_aacAudioConfig, (int)len);
				fileOut.write((char*)adtsheader, 7);
				fileOut.write((char*)pData, len);
			}
			pBuf->Unlock();
			pBuf->Release();
			pBuf = NULL;
			goto Again;
		}
		else if(hr == MF_E_TRANSFORM_NEED_MORE_INPUT)
		{
			return CodeOK;
		}
		else
		{
			LOG() << "mf aac encoder ProcessOutput " << std::hex << hr << std::dec;
			return CodeFalse;
		}
	}
	else
	{
		LOG() << "mf aac encoder ProcessInput " << std::hex << hr << std::dec;
		return CodeFalse;
	}

	return CodeOK;
}