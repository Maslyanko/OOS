#include "request_monitor.hpp"

#include <iostream>

namespace CHP {
    bool RequestMonitor::registration(std::string_view request) {
        std::unique_lock<std::shared_mutex> lock(mtx);
        if (requests.find(request) != requests.end()) {
            return false;
        }
        requests[request];
        return true;
    }

    void RequestMonitor::wait(std::string_view request) {
        std::shared_lock<std::shared_mutex> lock(mtx);
        if (requests.find(request) != requests.end()) {
            requests[request].wait(lock, [this, &request](){return requests.find(request) != requests.end();});
        }
    }

    void RequestMonitor::notify(std::string_view request) {
        std::unique_lock<std::shared_mutex> lock(mtx);
        if (requests.find(request) != requests.end()) {
            requests[request].notify_one();
        }
        requests.erase(request);
    }

    RequestMonitor::~RequestMonitor() {
        for (auto& requests: requests) {
            requests.second.notify_all();
        }
        std::cerr << "Monitor finished\n";
    }
}