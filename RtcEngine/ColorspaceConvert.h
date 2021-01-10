#pragma once
#include "libyuv/libyuv.h"
#include <memory>

class IConvertColorspace
{
public:
	virtual ~IConvertColorspace() {}
	virtual int ProcessInput(const uint8_t*, size_t) = 0;
	inline uint32_t GetInputWidth() { return m_iInputWidth; }
	inline uint32_t GetInputHeight() { return m_iInputHeight; }
	inline uint32_t GetInputType() { return m_iInputType; }
	inline uint32_t GetOutputSize() { return m_iOutSize; }
	inline uint8_t* GetOutputData() { return m_pBuf.get(); }

protected:
	virtual int ResetInputInfo(int w, int h, uint32_t type)
	{
		if (w < 0 || w > 10000 || w % 2 != 0 || h < 0 || h > 10000 || h % 2 != 0)
			return CodeFalse;

		m_iInputWidth = w;
		m_iInputHeight = h;
		m_iInputType = type;
		try
		{
			m_pBuf.reset(new uint8_t[w * h * 6]);
		}
		catch (...)
		{
			m_pBuf.reset(nullptr);
			return CodeFalse;
		}

		return CodeOK;
	}

protected:
	std::unique_ptr<uint8_t[]> m_pBuf;
	int m_iInputWidth = 0;
	int m_iInputHeight = 0;
	uint32_t m_iInputType = libyuv::FourCC::FOURCC_YUY2;
	uint32_t m_iOutSize = 0;
};

template<uint32_t SourceType>
class Convert2I420 : public IConvertColorspace
{
public:
	Convert2I420(int w, int h)
	{
		if (SourceType == libyuv::FOURCC_I420 || SourceType == libyuv::FOURCC_YV12)
		{
			LOG() << "no need to convert";
		}

		if (CodeOK != ResetInputInfo(w, h, SourceType))
		{
			LOG() << "Convert2I420::ResetInputInfo fail";
		}
	}

	int ResetInputInfo(int w, int h, uint32_t type) override
	{
		if (IConvertColorspace::ResetInputInfo(w, h, type) == CodeOK)
		{
			yStride = w;
			pY = m_pBuf.get();
			uStride = yStride / 2;
			pU = pY + w * h;
			vStride = uStride;
			pV = pU + w * h / 4;
			m_iOutSize = static_cast<uint32_t>(w * h * 1.5);
			return CodeOK;
		}

		return CodeFalse;
	}

	int ProcessInput(const uint8_t* pData, size_t size) override
	{
		if (!pData)
		{
			return CodeFalse;
		}

		int w = GetInputWidth();
		int h = GetInputHeight();

		if (SourceType == libyuv::FOURCC_NV12)
		{
			if (0 != libyuv::NV12ToI420(pData, w, pData + w * h, w, pY, yStride, pU, uStride, pV, vStride, w, h))
				return CodeFalse;
		}
		else if (SourceType == libyuv::FOURCC_YUY2)
		{
			if (0 != libyuv::YUY2ToI420(pData, w * 2, pY, yStride, pU, uStride, pV, vStride, w, h))
				return CodeFalse;
		}
		else if (SourceType == libyuv::FOURCC_UYVY)
		{
			if (0 != libyuv::UYVYToI420(pData, w * 2, pY, yStride, pU, uStride, pV, vStride, w, h))
				return CodeFalse;
		}
		else if (SourceType == libyuv::FOURCC_I400)
		{
			if (0 != libyuv::I400ToI420(pData, w, pY, yStride, pU, uStride, pV, vStride, w, h))
				return CodeFalse;
		}
		else
		{
			return CodeFalse;
		}

		return CodeOK;
	}

protected:
	int yStride = 0;
	int uStride = 0;
	int vStride = 0;
	uint8_t* pY = nullptr;
	uint8_t* pU = nullptr;
	uint8_t* pV = nullptr;
};