#include "common.h"

#ifdef _WIN32

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

#endif