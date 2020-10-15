#pragma once
#include <cassert>
#include <atomic>
#include <future>
#include "../common/common.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

class MFAACEncoder;
class MFCaptureAudio : public IMFSourceReaderCallback
{
public:
    MFCaptureAudio();
    ~MFCaptureAudio();
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP OnReadSample(
        HRESULT hrStatus,
        DWORD dwStreamIndex,
        DWORD dwStreamFlags,
        LONGLONG llTimestamp,
        IMFSample* pSample
    );
    STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*);
    STDMETHODIMP OnFlush(DWORD);

    int StartCapture(IMFActivate*, size_t streamIndex, size_t mediaIndex);
    int EndCapture();

protected:
    int ConfigureAudioSource(IMFMediaSource*, size_t streamIndex, size_t mediaIndex);
    int OpenMediaAudioSource(IMFMediaSource*);

protected:
    std::atomic<ULONG> m_nRefCount = 1;
    BOOL m_bCapture = FALSE;
    IMFMediaSource* m_pSource = NULL;
    IMFSourceReader* m_pReader = NULL;
    std::wstring m_strSymbolicLink;
    DWORD m_dwStreamIndex = 0;
    DWORD m_dwMediaIndex = 0;
    IMFTransform* m_pAudoSampleTransform = NULL;
    DWORD m_iCaptureSampleRate = 0;
    DWORD m_iCaptureSampleChannel = 0;
    DWORD m_iCaptureSampleBits = 0;
    MFT_OUTPUT_DATA_BUFFER m_resamleOutData = { 0 };
    MFAACEncoder* m_pAACEncoder = NULL;
    std::promise<bool> m_waitReadEnd;
};

class CMediaBuffer : public IMediaBuffer
{
private:
    DWORD        m_cbLength;
    const DWORD  m_cbMaxLength;
    LONG         m_nRefCount;  // Reference count
    BYTE* m_pbData;


    CMediaBuffer(DWORD cbMaxLength, HRESULT& hr) :
        m_nRefCount(1),
        m_cbMaxLength(cbMaxLength),
        m_cbLength(0),
        m_pbData(NULL)
    {
        m_pbData = new BYTE[cbMaxLength];
        if (!m_pbData)
        {
            hr = E_OUTOFMEMORY;
        }
    }

    ~CMediaBuffer()
    {
        if (m_pbData)
        {
            delete[] m_pbData;
        }
    }

public:

    // Function to create a new IMediaBuffer object and return 
    // an AddRef'd interface pointer.
    static HRESULT Create(long cbMaxLen, IMediaBuffer** ppBuffer)
    {
        HRESULT hr = S_OK;
        CMediaBuffer* pBuffer = NULL;

        if (ppBuffer == NULL)
        {
            return E_POINTER;
        }

        pBuffer = new CMediaBuffer(cbMaxLen, hr);

        if (pBuffer == NULL)
        {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            *ppBuffer = pBuffer;
            (*ppBuffer)->AddRef();
        }

        if (pBuffer)
        {
            pBuffer->Release();
        }
        return hr;
    }

    // IUnknown methods.
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (ppv == NULL)
        {
            return E_POINTER;
        }
        else if (riid == IID_IMediaBuffer || riid == IID_IUnknown)
        {
            *ppv = static_cast<IMediaBuffer*>(this);
            AddRef();
            return S_OK;
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_nRefCount);
    }

    STDMETHODIMP_(ULONG) Release()
    {
        LONG lRef = InterlockedDecrement(&m_nRefCount);
        if (lRef == 0)
        {
            delete this;
            // m_cRef is no longer valid! Return lRef.
        }
        return lRef;
    }

    // IMediaBuffer methods.
    STDMETHODIMP SetLength(DWORD cbLength)
    {
        if (cbLength > m_cbMaxLength)
        {
            return E_INVALIDARG;
        }
        m_cbLength = cbLength;
        return S_OK;
    }

    STDMETHODIMP GetMaxLength(DWORD* pcbMaxLength)
    {
        if (pcbMaxLength == NULL)
        {
            return E_POINTER;
        }
        *pcbMaxLength = m_cbMaxLength;
        return S_OK;
    }

    STDMETHODIMP GetBufferAndLength(BYTE** ppbBuffer, DWORD* pcbLength)
    {
        // Either parameter can be NULL, but not both.
        if (ppbBuffer == NULL && pcbLength == NULL)
        {
            return E_POINTER;
        }
        if (ppbBuffer)
        {
            *ppbBuffer = m_pbData;
        }
        if (pcbLength)
        {
            *pcbLength = m_cbLength;
        }
        return S_OK;
    }
};

class VoiceCapture
{
public:
    int StartCapture();
    int EndCapture();

protected:
    IMediaObject* m_pCapture = NULL;
    DMO_OUTPUT_DATA_BUFFER m_dataOut = { 0 };
};