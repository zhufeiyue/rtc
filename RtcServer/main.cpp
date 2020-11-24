#include <iostream>
#include "SrtServer.h"
#include "../common/SrtClient.h"
#include "../common/jsmn.h"
#include "../common/Eventloop.h"
#include "../common/SrtListener.h"
#include "SrtServer.h"

int main(int argc , char* argv[])
{
    srt_startup();

    auto pLoop = new Eventloop();
    auto pServer = new SrtServer(*pLoop, 2);
    pServer->Start("0.0.0.0", 8000);

    auto pClient = new SrtClient(*pLoop);
    pClient->Connect("127.0.0.1", 8000);

    pLoop->Run();

    srt_cleanup();
    return 0;
}
