#include <chrono>
#include "X264Wrap.h"

#ifdef _DEBUG
#pragma comment(lib, "./x264/libx264d.lib")
#else
#pragma comment(lib, "./x264/libx264.lib")
#endif

X264Wrap::X264Wrap()
{
}

X264Wrap::~X264Wrap()
{
}

void X264Wrap::SetFrameSize(int w, int h, int fr, int ff)
{
	if (w < 1 || w % 2 != 0 ||
		h < 1 || h % 2 != 0 ||
		fr < 1 || fr > 100 ||
		ff < X264_CSP_I400 || ff > X264_CSP_RGB)
	{
		LOG() << "x264 cannt take configure width " << w << " height " << h <<
			" frame rate " << fr << "frame format " << ff;
		return;
	}

	m_iWidth = w;
	m_iHeight = h;
	m_iFrameRate = fr;
	m_iFrameFormat = ff;
}

int X264Wrap::Init()
{
	if (m_iWidth == 0 || m_iHeight == 0 || m_iFrameRate == 0)
	{
		return CodeFalse;
	}
	if (m_pParam || m_pEncoder)
	{
		return CodeFalse;
	}

	m_pParam = new x264_param_t();
	if (0 != x264_param_default_preset(m_pParam, "ultrafast", "zerolatency"))
	{
		delete m_pParam;
		m_pParam = NULL;
		return CodeFalse;
	}

	m_pParam->i_bframe = 0;
	m_pParam->i_bitdepth = 8;
	m_pParam->i_csp = m_iFrameFormat; //X264_CSP_I420;
	m_pParam->i_width = m_iWidth;
	m_pParam->i_height = m_iHeight;
	m_pParam->b_vfr_input = 0; // 0 仅帧率用于码率控制，1 时间基和时间戳用于码率控制
	m_pParam->b_repeat_headers = 1; // 是否每个关键帧前放sps和pps
	m_pParam->b_aud = 0;
	m_pParam->b_annexb = 1; // 1 00000001 start code, 0 nale size
	m_pParam->i_fps_num = m_iFrameRate;
	m_pParam->i_fps_den = 1;
	m_pParam->i_keyint_min = m_iFrameRate * 2;
	m_pParam->i_keyint_max = m_iFrameRate * 300;
	m_pParam->i_scenecut_threshold;
	m_pParam->b_open_gop;
	m_pParam->b_bluray_compat;
	m_pParam->i_log_level = X264_LOG_NONE;
	m_pParam->b_opencl = 1;

	//m_pParam->rc.i_rc_method = X264_RC_ABR;
	//m_pParam->rc.i_bitrate = 600;

	m_pParam->rc.i_rc_method = X264_RC_CRF;
	m_pParam->rc.f_rf_constant = 28.0f;

	x264_param_apply_fastfirstpass(m_pParam);
	if (0 != x264_param_apply_profile(m_pParam, "baseline"))
	{
		LOG() << "fail to apply profile";
	}

	m_pEncoder = x264_encoder_open(m_pParam);
	if (!m_pEncoder)
	{
		return CodeFalse;
	}

	x264_picture_init(&m_picOut);
	x264_picture_alloc(&m_picIn, m_pParam->i_csp, m_pParam->i_width, m_pParam->i_height);

	return CodeOK;
}

int X264Wrap::Destroy()
{
	if (!m_pEncoder)
	{
		return CodeOK;
	}
	x264_encoder_close(m_pEncoder);
	m_pEncoder = NULL;

	if (m_pParam)
	{
		x264_param_cleanup(m_pParam);
		delete m_pParam;
		m_pParam = NULL;
	}

	return CodeOK;
}

int X264Wrap::ProcessInput(uint8_t* yuv[], int)
{
	x264_nal_t* nal = NULL;
	int i_nal = 0;

	if (!m_pEncoder)
	{
		return CodeFalse;
	}

	memcpy(m_picIn.img.plane[0], yuv[0], m_picIn.img.i_stride[0] * m_iHeight);
	memcpy(m_picIn.img.plane[1], yuv[1], m_picIn.img.i_stride[1] * m_iHeight/2);
	memcpy(m_picIn.img.plane[2], yuv[2], m_picIn.img.i_stride[2] * m_iHeight/2);

	auto size = x264_encoder_encode(m_pEncoder, &nal, &i_nal, &m_picIn, &m_picOut);
	if (size < 0)
	{
		LOG() << "encode error " << size;
		return CodeFalse;
	}
	if (size > 0)
	{
		static std::ofstream fileOut;
		if (!fileOut.is_open())
		{
			fileOut.open("d:\\1.h264", std::ofstream::binary | std::ofstream::out);
		}
		if (fileOut.is_open())
		{
			fileOut.write((const char*)nal->p_payload, size);
		}
	}

	return CodeOK;
}