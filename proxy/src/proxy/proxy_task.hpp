#pragma once

#include "proxy/proxy.hpp"
#include "../multithreading/thread_pool.hpp"

#include <string_view>
#include <memory>

namespace CHP {
    class ProxyTask: public MT::Task {
        int readFrom(int fd, std::unique_ptr<char[]>& buffer);
        int writeTo(int fd, std::unique_ptr<char[]>& buffer, int length);
        int connectToServer(std::string_view address, int port);
    public:
        ProxyTask(int client);
        void execute() override;
        ~ProxyTask() override;
    private:
        int client;
        int server = -1;

        const int startBufferSize = 1024;
    };
} // namesace CachingHttpProxy