#include "cache.hpp"
#include <iostream>

namespace cache {
    void Cache::garbageCollector() {
        while (true) {
            std::unique_lock<std::shared_mutex> lock(mtx);
            auto nextCheck = std::chrono::system_clock::now() + std::chrono::seconds(ttl);
            if(shutDownVar.wait_until(lock, nextCheck) == std::cv_status::timeout) {
                for (auto it = cache.begin(); it != cache.end();) {
                    if (std::chrono::system_clock::now() - it->second.lastUsed > std::chrono::seconds(ttl)) {
                        size -= it->second.requestSize + it->second.responseSize;
                        it = cache.erase(it);
                    } else {
                        ++it;
                    }
                }
            } else {
                break;
            }
        }
    }

    Cache::Cache(size_t minSize, size_t maxSize, size_t ttl) {
        this->minSize = minSize * 1024 * 1024;
        this->maxSize = maxSize * 1024 * 1024;
        this->ttl = ttl;

        size = 0;

        garbageCollectorThread = std::thread(&Cache::garbageCollector, this);
    }

    void Cache::insert(
        std::shared_ptr<char[]> request, 
        size_t requestSize, 
        std::shared_ptr<char[]> response, 
        size_t responseSize
    ) {
        std::unique_lock<std::shared_mutex> lock(mtx);

        if (cache.find(std::string_view(request.get(), requestSize)) != cache.end()) {
            return;
        }

        size_t entrySize = requestSize + responseSize;
        if (entrySize > maxSize) {
            return;
        }

        if (size + entrySize > maxSize) {
            auto it = cache.begin();
            while (it != cache.end()) {
                if (size + entrySize <= maxSize) {
                    break;
                }
                size -= it->second.requestSize + it->second.responseSize;
                it = cache.erase(it);
            }
        }

        cache[std::string_view(request.get(), requestSize)] = {
            request, 
            requestSize, 
            response, 
            responseSize, 
            std::chrono::system_clock::now(),
        };
    }

    std::pair<std::shared_ptr<char[]>, size_t> Cache::get(std::shared_ptr<char[]> request,size_t requestSize) {
        std::shared_lock<std::shared_mutex> lock(mtx);
        auto it = cache.find(std::string_view(request.get(), requestSize));
        if (it!= cache.end()) {
            it->second.lastUsed = std::chrono::system_clock::now();
            return {it->second.response, it->second.requestSize};
        }
        return {nullptr, 0};
    }

    Cache::~Cache() {
        shutDownVar.notify_all();
        garbageCollectorThread.join();
        std::cerr << "Cache terminated\n";
    }
}