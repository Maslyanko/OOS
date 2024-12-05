#pragma once

#include <unordered_map>
#include <string_view>
#include <shared_mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <condition_variable>

namespace cache {
    class Cache {
        struct Entry {
            std::shared_ptr<char[]> request;
            size_t requestSize;
            std::shared_ptr<char[]> response;
            size_t responseSize;
            std::chrono::time_point<std::chrono::system_clock> lastUsed;
        };
        void garbageCollector();
    public:
        Cache(size_t minSize, size_t maxSize, size_t ttl);

        void insert(
            std::shared_ptr<char[]> request, 
            size_t requestSize, 
            std::shared_ptr<char[]> response, 
            size_t responseSize
        );
        std::pair<std::shared_ptr<char[]>, size_t> get(std::shared_ptr<char[]> request, size_t requestSize);

        ~Cache();
    private:
        size_t minSize;
        size_t maxSize;
        size_t ttl;

        size_t size;

        std::unordered_map<std::string_view, Entry> cache;
        std::shared_mutex mtx;

        std::condition_variable_any shutDownVar;

        std::thread garbageCollectorThread;
    };
} // namespace cache