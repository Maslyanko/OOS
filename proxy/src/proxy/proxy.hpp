#pragma once
#include "../multithreading/thread_pool.hpp"
#include "proxy_task.hpp"

namespace CHP {
    class Proxy {
    public:
        Proxy(int port, int maxClientsThreads);

        void start();
        void shutdown();
        
        ~Proxy();
    private:
        int listenSocket;
        MT::ThreadPool workers;
    };
} // namespace CachingHttpProxy