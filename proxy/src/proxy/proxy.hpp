#pragma once
#include "../multithreading/thread_pool.hpp"
#include "../cache/cache.hpp"
#include "request_monitor.hpp"
#include "proxy_task.hpp"

#include <memory>

namespace CHP {
    class Proxy {
    public:
        Proxy(
            int port, 
            int maxClientsThreads,
            int minCacheSize,
            int maxCacheSize,
            int cacheTtl
        );

        void start();
        void shutdown();
        
        ~Proxy();
    private:
        int listenSocket;

        std::shared_ptr<RequestMonitor> monitor;
        std::shared_ptr<cache::Cache> cache;
        std::shared_ptr<MT::ThreadPool> workers;

        bool shutDown;
    };
} // namespace CachingHttpProxy