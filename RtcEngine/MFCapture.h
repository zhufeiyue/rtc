#pragma once
#include <cassert>
#include <atomic>
#include "common.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

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
    BOOL m_bCapture = TRUE;
    IMFMediaSource* m_pSource = NULL;
    IMFSourceReader* m_pReader = NULL;
    std::wstring m_strSymbolicLink;
    DWORD m_dwStreamIndex = 0;
    DWORD m_dwMediaIndex = 0;
    IMFTransform* m_pAudoSampleTransform = NULL;
    MFT_OUTPUT_DATA_BUFFER m_resamleOutData = { 0 };
};

