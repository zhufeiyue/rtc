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

    auto pQueue = new EventQueueLockFree();
    //auto pQueue = new EventQueueMutex();
    pQueue->PushEvent(
        []() {
            std::cout << "1";
        });
    pQueue->PushEvent(
        []() {
            std::cout << "2";
        });
    pQueue->PushEvent(
        []() {
            std::cout << "3";
        });
    Fn temp = []() {
        std::cout << "4";
    };
    int n = 0;
    pQueue->PopAll(
        [&n, temp, pQueue](Fn& f) {
            f();
            if (n == 0) {
                pQueue->PushEvent(Fn(temp));
                n = 1;
            }
        }
    );

    auto pLoop = new Eventloop();
    auto pServer = new SrtServer(*pLoop, 1);
    pServer->Start("0.0.0.0", 8000);
    pLoop->Run();


    srt_cleanup();
    return 0;
}
