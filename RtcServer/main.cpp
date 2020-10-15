#include <iostream>
#include "SrtServer.h"
#include "../common/SrtClient.h"

int main(int argc , char* argv[])
{
    srt_startup();

    auto p = new SrtServer();
    p->Create();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto pSrtClient = new SrtClient();
    pSrtClient->Connect("127.0.0.1", 8000);

    std::this_thread::sleep_for(std::chrono::seconds(50));
    p->Destroy();

    srt_cleanup();
    return 0;
}
