#include <iostream>
#include "SrtServer.h"
#include "../common/SrtClient.h"
#include "../common/jsmn.h"
#include "../common/EventQueue.h"

int main(int argc , char* argv[])
{
    srt_startup();

    auto pQueue = new EventQueueLockFree();
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
