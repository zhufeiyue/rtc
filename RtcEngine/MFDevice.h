#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <map>
#include <mfidl.h>
#include "../common/common.h"

struct DesiredVideoConfig
{
	int width = 640;
	int height = 360;
	float frameRate = 15;
};

struct DesiredAudioConfig
{
	int sampleRate = 0;
};

class MFDevice
{
public:
	struct MediaInfo
	{
		UINT32 m_frameWidth = 0;
		UINT32 m_frameHeight = 0;
		float m_frameRate = 0;
		float m_minFrameRate = 0;
		float m_maxFrameRate = 0;

		UINT32 m_audioSampleRate = 0;
		UINT32 m_audioChannelNum = 0;
		UINT32 m_audioSampleBits = 0;
		UINT32 m_audioBlockAlign = 0;

		BOOL m_bIsCompress = FALSE;
		GUID m_majorType;
		GUID m_subType;
	};

	struct StreamInfo
	{
		StreamInfo() {}
		StreamInfo(StreamInfo&& rr)
		{
			m_vMediaInfo = std::move(rr.m_vMediaInfo);
		}

		std::vector<MediaInfo> m_vMediaInfo;
	};

	struct DeviceInfo
	{
		std::wstring m_strFriendlyName;
		std::wstring m_strSymbolicLink;
		std::vector<StreamInfo> m_vStreamInfo;
	};

	MFDevice();
	~MFDevice();
	HRESULT EnumDevice(IID);
	IMFActivate* GetDeviceActivate(size_t);
	int ChooseVideo(DesiredVideoConfig, size_t deviceIndex, size_t& streamIndex, size_t& mediaIndex);
	int ChooseAudio(DesiredAudioConfig, size_t deviceIndex, size_t& streamIndex, size_t& mediaIndex);

protected:
	HRESULT GetDeviceInfo(IMFActivate*);
	HRESULT GetDeviceStreamInfo(IMFStreamDescriptor*, StreamInfo& );
	HRESULT GetDeviceStreamMediaInfo(IMFMediaType*, MediaInfo&);

protected:
	IMFAttributes* m_pAttributes = NULL;
	IMFActivate** m_ppDevices = NULL;

	UINT32 m_iDeviceCount = 0;
	GUID m_guidDeviceType = { 0 };
	std::vector<DeviceInfo> m_vDevicesInfo;
};

