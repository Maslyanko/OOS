#pragma once

#include <unordered_map>
#include <string_view>
#include <condition_variable>
#include <shared_mutex>

namespace CHP {
    class RequestMonitor {
    public:
        bool registration(std::string_view request);
        void notify(std::string_view request);
        void wait(std::string_view request);
        ~RequestMonitor();
    private:
        std::shared_mutex mtx;
        std::unordered_map<std::string_view, std::condition_variable_any> requests;
    };
} // namespace CachingHttpProxy