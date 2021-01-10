#pragma once
#include "../common/common.h"
#include "x264/x264.h"

class X264Wrap
{
public:
	X264Wrap();
	~X264Wrap();

	void SetFrameSize(int w, int h, int fr, int ff = X264_CSP_I420);

	int Init();
	int Destroy();
	int ProcessInput(uint8_t* [], int);

protected:
	x264_param_t* m_pParam = NULL;
	x264_t* m_pEncoder = NULL;
	x264_picture_t m_picIn = { 0 };
	x264_picture_t m_picOut = { 0 };
	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_iFrameRate = 0;
	int m_iFrameFormat = X264_CSP_I420;
};

