#include "MFCapture.h"
#include <strsafe.h>
#include <Mferror.h>
#include <Wmcodecdsp.h>
#include <propvarutil.h>
#include <shlwapi.h>

HRESULT LogMediaType(IMFMediaType* pType);
#define ResampleOutputRate 44100
#define ResampleOutputChannel 2
#define ResampleOutputBits 16

MFCaptureAudio::MFCaptureAudio()
{
}

MFCaptureAudio::~MFCaptureAudio()
{
    if (m_pAudoSampleTransform)
        m_pAudoSampleTransform->Release();
    if (m_resamleOutData.pSample)
        m_resamleOutData.pSample->Release();
}

HRESULT MFCaptureAudio::QueryInterface(REFIID iid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(MFCaptureAudio, IMFSourceReaderCallback),
        { 0 },
    };
    return QISearch(this, qit, iid, ppv);
}

ULONG MFCaptureAudio::AddRef()
{
    auto count = m_nRefCount++;
    return count;
}

ULONG MFCaptureAudio::Release()
{
    auto count = m_nRefCount--;
    if (count == 0)
    {
        delete this;
    }
    return count;
}

HRESULT MFCaptureAudio::OnReadSample(
    HRESULT hrStatus,
    DWORD dwStreamIndex,
    DWORD dwStreamFlags,
    LONGLONG llTimestamp,
    IMFSample* pSample)
{
    HRESULT hr;
    DWORD len(0);
    DWORD dwStatus(0);
    if (pSample)
    {
        if (m_pAudoSampleTransform)
        {
            hr = m_pAudoSampleTransform->ProcessInput(0, pSample, 0);
            if (SUCCEEDED(hr))
            {
                hr = m_pAudoSampleTransform->ProcessOutput(0, 1, &m_resamleOutData, &dwStatus);
                if (SUCCEEDED(hr))
                {
                    if (m_resamleOutData.pEvents)
                        m_resamleOutData.pEvents->Release();
                    m_resamleOutData.pSample->GetTotalLength(&len);
                }
                else if (MF_E_TRANSFORM_STREAM_CHANGE == hr)
                    LOG() << "MF_E_TRANSFORM_STREAM_CHANGE";
                else
                    LOG() << hr;
            }
        }
    }
End:
    if (m_bCapture && m_pReader)
        hr = m_pReader->ReadSample(m_dwStreamIndex, 0, NULL, NULL, NULL, NULL);
    return S_OK;
}

HRESULT MFCaptureAudio::OnEvent(DWORD, IMFMediaEvent* pEvent)
{
    HRESULT hr;
    MediaEventType met;

    hr = pEvent->GetType(&met);
    if (SUCCEEDED(hr))
    {
        PROPVARIANT var;
        PropVariantInit(&var);

        if (met == MEStreamTick)
        {
            pEvent->GetValue(&var);
            std::cout << " stream tick " << var.hVal.QuadPart << std::endl;
        }

        PropVariantClear(&var);
    }

    return S_OK;
}

HRESULT MFCaptureAudio::OnFlush(DWORD)
{
    return S_OK;
}

int MFCaptureAudio::StartCapture(IMFActivate* pActivate, size_t streamIndex, size_t mediaIndex)
{
    if (!pActivate)
    {
        return CodeFalse;
    }

    HRESULT hr;
    IMFMediaSource* pSource = NULL;
    UINT32 len(0);
    WCHAR* wszName = NULL;
    hr = pActivate->ActivateObject(__uuidof(IMFMediaSource), (void**)&pSource);
    if (SUCCEEDED(hr))
    {
        hr = pActivate->GetAllocatedString(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID,
            &wszName,
            &len
        );
        if (wszName)
        {
            m_strSymbolicLink = std::wstring(wszName, len);
            CoTaskMemFree(wszName);
        }
    }

    m_dwStreamIndex = streamIndex;
    m_dwMediaIndex = mediaIndex;
    if (CodeOK != ConfigureAudioSource(pSource, streamIndex, mediaIndex) || 
        CodeOK != OpenMediaAudioSource(pSource))
    {
        return CodeFalse;
    }
    m_pSource = pSource;

    return CodeOK;
}

int MFCaptureAudio::EndCapture()
{
    m_bCapture = FALSE;
    return CodeOK;
}

int MFCaptureAudio::ConfigureAudioSource(IMFMediaSource* pSource, size_t streamIndex, size_t mediaIndex)
{
    HRESULT hr = S_FALSE;
    IMFPresentationDescriptor* pPD = NULL;
    IMFStreamDescriptor* pSD = NULL;
    IMFMediaTypeHandler* pHandler = NULL;
    IMFMediaType* pType = NULL;
    BOOL bSelect = FALSE;

    hr = pSource->CreatePresentationDescriptor(&pPD);
    if (SUCCEEDED(hr))
    {
        hr = pPD->GetStreamDescriptorByIndex(streamIndex, &bSelect, &pSD);
        if (SUCCEEDED(hr))
        {
            if (!bSelect)
                hr = pPD->SelectStream(streamIndex);
        }
    }
    if (SUCCEEDED(hr))
    {
        hr = pSD->GetMediaTypeHandler(&pHandler);
    }
    if (SUCCEEDED(hr))
    {
        hr = pHandler->GetMediaTypeByIndex(mediaIndex, &pType);
    }
    if (SUCCEEDED(hr))
    {
        hr = pHandler->SetCurrentMediaType(pType);
    }
    if (SUCCEEDED(hr))
    {
        GUID guidSubType;
        UINT32 audioSampleRate = 0;
        UINT32 audioChannelNum = 0;
        UINT32 audioChannelMask = 0;
        UINT32 audioSampleBits = 0;
        UINT32 audioBlockAlign = 0;
        UINT32 audioBytesPerSecond = 0;

        hr = pType->GetGUID(MF_MT_SUBTYPE, &guidSubType);
        hr = pType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &audioSampleRate);
        hr = pType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &audioChannelNum);
        hr = pType->GetUINT32(MF_MT_AUDIO_CHANNEL_MASK, &audioChannelMask);
        hr = pType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &audioSampleBits);
        hr = pType->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &audioBlockAlign);
        hr = pType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &audioBytesPerSecond);
        if (audioBlockAlign == 0)
        {
            audioBlockAlign = audioSampleBits / 8 * audioSampleRate;
        }
        if (audioBytesPerSecond == 0)
        {
            audioBytesPerSecond = audioBlockAlign * audioSampleRate;
        }

        if (guidSubType != MFAudioFormat_PCM ||
            audioSampleBits != 16 ||
            (audioChannelNum != 2 && audioChannelNum != 1) ||
            (audioSampleRate != 44100 && audioSampleRate != 48000))
        {
            hr = CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER, IID_IMFTransform, (void**)&m_pAudoSampleTransform);
            if (SUCCEEDED(hr))
            {
                IWMResamplerProps* pWMResamplerProps = NULL;
                IMFMediaType* pInType (NULL), *pOutType(NULL);
                hr = m_pAudoSampleTransform->QueryInterface(__uuidof(IWMResamplerProps), (void**)&pWMResamplerProps);
                if (SUCCEEDED(hr))
                {
                    pWMResamplerProps->SetHalfFilterLength(60);
                }

                hr = MFCreateMediaType(&pInType);
                hr = pInType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
                hr = pInType->SetGUID(MF_MT_SUBTYPE, guidSubType);
                hr = pInType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, audioChannelNum);
                hr = pInType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, audioSampleRate);
                hr = pInType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, audioSampleBits);
                hr = pInType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, audioBlockAlign);
                hr = pInType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
                hr = pInType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, audioBytesPerSecond);
                if (audioChannelMask != 0)
                    hr = pInType->SetUINT32(MF_MT_AUDIO_CHANNEL_MASK, audioChannelMask);
                hr = m_pAudoSampleTransform->SetInputType(0, pInType, 0);
                if (SUCCEEDED(hr))
                {
                    hr = MFCreateMediaType(&pOutType);
                    hr = pOutType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
                    hr = pOutType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
                    hr = pOutType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, ResampleOutputChannel);
                    hr = pOutType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, ResampleOutputRate);
                    hr = pOutType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, ResampleOutputBits);
                    hr = pOutType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, ResampleOutputChannel * ResampleOutputBits / 8);
                    hr = pOutType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, ResampleOutputRate * ResampleOutputChannel * ResampleOutputBits / 8);
                    hr = pOutType->SetUINT32(MF_MT_AUDIO_CHANNEL_MASK, 3);
                    hr = m_pAudoSampleTransform->SetOutputType(0, pOutType, 0);
                    if (SUCCEEDED(hr))
                    {
                        MFT_OUTPUT_STREAM_INFO outInfo;
                        IMFMediaBuffer* pBuffer = NULL;
                        hr = m_pAudoSampleTransform->GetOutputStreamInfo(0, &outInfo);
                        hr = MFCreateSample(&(m_resamleOutData.pSample));
                        hr = MFCreateMemoryBuffer(ResampleOutputRate * ResampleOutputChannel * ResampleOutputBits / 8, &pBuffer);
                        if (SUCCEEDED(hr))
                        {
                            hr = m_resamleOutData.pSample->AddBuffer(pBuffer);
                            pBuffer->Release();
                            pBuffer = NULL;
                        }
                        else
                        {
                            m_resamleOutData.pSample->Release();
                            m_resamleOutData.pSample = NULL;
                        }
                    }
                }

                if (pWMResamplerProps)
                    pWMResamplerProps->Release();
                if (pInType)
                    pInType->Release();
                if (pOutType)
                    pOutType->Release();
            }
        }
    }

    if (pType)
        pType->Release();
    if (pHandler)
        pHandler->Release();
    if (pSD)
        pSD->Release();
    if (pPD)
        pPD->Release();
    if (SUCCEEDED(hr))
        return CodeOK;
    else
        return CodeFalse;
}

int MFCaptureAudio::OpenMediaAudioSource(IMFMediaSource* pSource)
{
    HRESULT hr;
    IMFAttributes* pAttribute = NULL;
    hr = MFCreateAttributes(&pAttribute, 1);

    if (SUCCEEDED(hr))
    {
        hr = pAttribute->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFCreateSourceReaderFromMediaSource(
            pSource,
            pAttribute,
            &m_pReader
        );

        if (SUCCEEDED(hr))
        {
            BOOL bSelect(FALSE);
            m_pReader->GetStreamSelection(m_dwStreamIndex, &bSelect);
            if (!bSelect)
            {
                hr = m_pReader->SetStreamSelection(m_dwStreamIndex, true);
            }
        }

        //IMFMediaType* pCurrentType = NULL;
        //hr = m_pReader->GetCurrentMediaType(m_dwStreamIndex, &pCurrentType);
        //LogMediaType(pCurrentType);
        //pCurrentType->Release();

        IMFMediaType* pNativeType = NULL;
        hr = m_pReader->GetNativeMediaType(m_dwStreamIndex, m_dwMediaIndex, &pNativeType);

        if (SUCCEEDED(hr))
        {
            hr = m_pReader->SetCurrentMediaType(m_dwStreamIndex, NULL, pNativeType);
            pNativeType->Release();
        }

        if (SUCCEEDED(hr))
        {
            m_bCapture = TRUE;
            hr = m_pReader->ReadSample(m_dwStreamIndex, 0, NULL, NULL, NULL, NULL);
        }
    }

    if (pAttribute)
    {
        pAttribute->Release();
    }
    return CodeOK;
}