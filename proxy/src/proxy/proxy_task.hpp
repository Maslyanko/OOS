#pragma once

#include "proxy/proxy.hpp"
#include "request_monitor.hpp"
#include "../multithreading/thread_pool.hpp"
#include "../cache/cache.hpp"

#include <string_view>
#include <memory>

namespace CHP {
    class ProxyTask: public MT::Task {
        int readFrom(int fd, std::shared_ptr<char[]>& buffer);
        int writeTo(int fd, std::shared_ptr<char[]>& buffer, int length);
        int connectToServer(std::string_view address, int port);
    public:
        ProxyTask(
            int client,
            std::shared_ptr<RequestMonitor>& monitor,
            std::shared_ptr<cache::Cache>& cache,
            std::shared_ptr<MT::ThreadPool>& workers,
            bool& shutdown
        );
        void execute() override;
        ~ProxyTask() override;
    private:
        int client;
        int server = -1;

        const int startBufferSize = 1024;

        bool& shutdown;
        std::shared_ptr<RequestMonitor> monitor;
        std::shared_ptr<cache::Cache> cache;
        std::shared_ptr<MT::ThreadPool> workers;
    };
} // namesace CachingHttpProxy