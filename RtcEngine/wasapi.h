#pragma once

#include "../common/common.h"

#include <windows.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <propsys.h>

namespace Wasapi
{
	struct AudioDeviceInfo 
	{
		std::wstring name;
		std::wstring id;
	};

	// input true 如麦克风输入
	// input false 如扬声器输出
	void GetWASAPIAudioDevices(std::vector<AudioDeviceInfo>& devices, bool input);

	class WASAPICapture
	{
	public:
		enum class CaptureType
		{
			Input,    // capturte
			Render,   // output play
			Loopback, // output capture
		};

		typedef std::function<int(uint8_t* , uint32_t , uint64_t , uint64_t )> AudioCb;

		WASAPICapture(CaptureType type);
		virtual ~WASAPICapture();

		inline void SetAudioDeviceId(std::wstring id)
		{
			m_strDeviceId = std::move(id);
		}
		inline void SetAudioCb(AudioCb cb)
		{
			m_audioCb = cb;
		}
		inline const WAVEFORMATEXTENSIBLE& AudioFormat() const
		{
			return m_format;
		}
		inline void SetAudioFormat(WAVEFORMATEXTENSIBLE f)
		{
			m_format = f;
		}

		int Init();
		int Destroy();

	protected:
		virtual int InitClient();
		virtual int InitService();
		virtual int ProcessData();
		int WorkLoop();

	protected:
		CaptureType m_captureType = CaptureType::Input;
		AudioCb m_audioCb;

		std::wstring m_strDeviceId = L"default";
		IMMDevice* m_pDevice = nullptr;
		IAudioClient* m_pClient = nullptr;
		IAudioCaptureClient* m_pClientCapture = nullptr;
		IAudioRenderClient* m_pClientRender = nullptr;
		uint32_t m_devcieSample = 48000;
		WAVEFORMATEXTENSIBLE m_format = { 0 };

		bool m_bWorking = false;
		std::thread m_threadWork;
	};

	class WASAPILoopbackCapture : public WASAPICapture
	{
	public:
		WASAPILoopbackCapture() : WASAPICapture(WASAPICapture::CaptureType::Loopback) {}
	};

	class WASAPIPlay : public WASAPICapture
	{
	public:
		WASAPIPlay() :WASAPICapture(WASAPICapture::CaptureType::Render) {}

	protected:
		int InitClient() override;
		int InitService() override;
		int ProcessData() override;
	};
};

class WavMuxer
{
public:
	WavMuxer(WAVEFORMATEXTENSIBLE f):m_format(f) {}
	virtual ~WavMuxer() {}
	int WriteRIFF(uint32_t size) // 12 bytes
	{
		uint32_t chunkSize = 4 + size;
		riffSize = 12;

		Seek(0);
		Write("RIFF", 4);
		Write(&chunkSize, 4);
		Write("WAVE", 4);
		// size byes data 
		return CodeOK;
	}
	int Writefmt(uint32_t) // 26 or 48 bytes
	{
		uint32_t chunkSize = 18; // 16 18 40
		uint16_t extendSize = 0;

		if (m_format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
			extendSize = 22;
		else
			extendSize = 0;
		chunkSize = 18 + extendSize;
		fmtSize = 8 + 18 + extendSize;

		Seek(riffSize);
		Write("fmt ", 4);
		Write(&chunkSize, 4);

		Write(&m_format.Format.wFormatTag, 2);
		Write(&m_format.Format.nChannels, 2);
		Write(&m_format.Format.nSamplesPerSec, 4);
		Write(&m_format.Format.nAvgBytesPerSec, 4);
		Write(&m_format.Format.nBlockAlign, 2);
		Write(&m_format.Format.wBitsPerSample, 2);
		Write(&extendSize, 2);

		if (extendSize != 0)
		{
			Write(&m_format.Samples.wValidBitsPerSample, 2);
			Write(&m_format.dwChannelMask, 4);
			Write(&m_format.SubFormat, 16);
		}

		return CodeOK;
	}
	int Writefact(uint32_t sampleCount)
	{
		// is fact chunk not necessary?
		if (m_format.Format.wFormatTag == WAVE_FORMAT_PCM || true)
		{
			factSize = 0;
			return CodeOK;
		}
		factSize = 8;

		Seek(riffSize + fmtSize);
		Write("fact", 4);
		Write(&sampleCount, 4);

		return CodeOK;
	}
	int Writedata(uint32_t dataSize)
	{
		Seek(riffSize + fmtSize + factSize);
		Write("data", 4);
		Write(&dataSize, 4);
		return CodeOK;
	}
	int WriteSample(uint32_t frames, uint8_t* pData, size_t size)
	{
		m_iSampleCount += frames;
		Write(pData, size);
		return CodeOK;
	}
	int Flush()
	{
		int32_t dataSize = SampleCount() * m_format.Format.nBlockAlign;
		Writedata(dataSize);
		Writefact(SampleCount());
		WriteRIFF(dataSize + 8 + fmtSize + factSize + 4);

		return CodeOK;
	}
	uint32_t SampleCount()
	{
		return m_iSampleCount;
	}
protected:
	virtual int Seek(int32_t)
	{
		return CodeOK;
	}
	virtual int Write(const void*, size_t len)
	{
		return CodeOK;
	}

protected:
	WAVEFORMATEXTENSIBLE m_format = { 0 };
	int16_t riffSize = 0;
	int16_t fmtSize = 0;
	int16_t factSize = 0;
	uint32_t m_iSampleCount = 0;
};

class WavMuxerFile : public WavMuxer
{
public:
	WavMuxerFile(WAVEFORMATEXTENSIBLE f, std::string fileName) :WavMuxer(f)
	{
		m_file.open(fileName,   std::fstream::out | std::fstream::binary);
		LOG() << m_file.is_open() << '-' << GetLastError();
	}

protected:
	int Seek(int32_t offset) override
	{
		if (!m_file.is_open() || m_file.bad())
			return CodeFalse;

		m_file.seekp(offset, std::fstream::beg);
		//LOG() << m_file.tellp();

		return CodeOK;
	}
	virtual int Write(const void*pData, size_t len)
	{
		if (!m_file.is_open() || m_file.bad())
			return CodeFalse;

		m_file.write((const char*)pData, len);

		return CodeOK;
	}

protected:
	std::fstream m_file;
};

class WavDeMuxerFile
{
public:
	WavDeMuxerFile(std::string fileName)
	{
		m_fileIn.open(fileName, std::fstream::in | std::fstream::binary);
	}

	inline const WAVEFORMATEXTENSIBLE& AudioFormat() const
	{
		return m_format;
	}

	int ReadFormat()
	{
		uint32_t  got = 0, n = 0;
		uint8_t buf[256] = { 0 };

		if (CodeOK != Seek(0))
		{
			return CodeOK;
		}

		// RIFF
		if (CodeOK != Read(buf, 8, got) || got != 8)
		{
			return CodeOK;
		}
		if (0 != strncmp((char*)buf, "RIFF", 4))
		{
			return CodeFalse;
		}
		m_iRIFFChunkSize = *(uint32_t*)(buf + 4);

		// WAVE fmt
		if (CodeOK != Read(buf, 12, got) || got != 12)
		{
			return CodeOK;
		}
		if (0 != strncmp((char*)buf, "WAVE", 4) ||
			0 != strncmp((char*)buf + 4, "fmt ", 4))
		{
			return CodeFalse;
		}
		m_iFmtChunkSize = *(uint32_t*)(buf + 8); // 16 18 or 40
		if (CodeOK != Read(buf, m_iFmtChunkSize, got) || got != m_iFmtChunkSize)
		{
			return CodeFalse;
		}
		m_format.Format = *(WAVEFORMATEX*)buf;
		if (m_iFmtChunkSize == 40)
		{
			m_format.Samples.wValidBitsPerSample = *(uint16_t*)(buf + 18); // 18 is sizeof(WAVEFORMATEX)
			m_format.dwChannelMask = *(uint32_t*)(buf + 20);
			m_format.SubFormat = *(GUID*)(buf + 24);
		}

		bool gotDataChunk = false;
		int tryCount = 0;
		do
		{
			if (CodeOK != Read(buf, 8, got) || got != 8)
			{
				return CodeFalse;
			}
			if (strncmp((char*)buf, "data", 4) == 0)
			{
				gotDataChunk = true;
				m_iDataChunkSize = *(uint32_t*)(buf + 4);
				m_iSampleCount = m_iDataChunkSize / m_format.Format.nBlockAlign;
				return CodeOK;
			}
			
			n = *(uint32_t*)(buf + 4);
			if (CodeOK != Seek(n, std::fstream::cur))
			{
				return CodeFalse;
			}

		} while (!gotDataChunk && ++tryCount <= 3);

		return CodeFalse;
	}

	int ReadSample( uint8_t* buf, uint32_t bufSize, uint32_t& bufGot)
	{
		if (m_format.Format.nBlockAlign == 0)
		{
			return CodeFalse;
		}

		return Read(buf, bufSize / m_format.Format.nBlockAlign * m_format.Format.nBlockAlign, bufGot);
	}

protected:
	int Seek(uint32_t offset, int type = std::fstream::beg)
	{
		if (!m_fileIn.is_open() || m_fileIn.bad())
		{
			return CodeFalse;
		}

		m_fileIn.seekg(offset, type);
		return CodeOK;
	}
	int Read(uint8_t* pBuf, uint32_t want, uint32_t& got)
	{
		if (!m_fileIn.is_open() || m_fileIn.bad() || m_fileIn.eof())
		{
			return CodeFalse;
		}

		m_fileIn.read((char*)pBuf, want);
		if (m_fileIn.fail())
		{
			got = 0;
			return CodeOK;
		}

		got = (uint32_t)m_fileIn.gcount();

		return CodeOK;
	}

protected:
	WAVEFORMATEXTENSIBLE m_format = { 0 };
	uint32_t m_iRIFFChunkSize = 0;
	uint32_t m_iFmtChunkSize = 0;
	uint32_t m_iDataChunkSize = 0;
	uint32_t m_iSampleCount = 0;
	std::fstream m_fileIn;
};