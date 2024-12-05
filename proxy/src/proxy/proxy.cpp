#include "proxy.hpp"
#include "../util/error/error.hpp"
#include "proxy_task.hpp"

#include <iostream>
#include <arpa/inet.h>

namespace CHP {
    Proxy::Proxy(int port, 
            int maxClientsThreads,
            int minCacheSize,
            int maxCacheSize,
            int cacheTtl
    ) {
        if (port < 0 || port > 65535) {
            throw util::Exception("Invalid port number");
        }

        listenSocket = util::cErrHandler("Socket: ", socket(AF_INET, SOCK_STREAM, 0));

        sockaddr_in proxyAddr;
        proxyAddr.sin_family = AF_INET;
        proxyAddr.sin_port = htons(port);
        proxyAddr.sin_addr.s_addr = INADDR_ANY;

        util::cErrHandler("Bind: ", bind(listenSocket, (sockaddr*)&proxyAddr, sizeof(proxyAddr)));

        monitor = std::make_shared<RequestMonitor>();
        cache = std::make_shared<cache::Cache>(minCacheSize, maxCacheSize, cacheTtl);
        workers = std::make_shared<MT::ThreadPool>(maxClientsThreads);
    };

    void Proxy::start() {
        util::cErrHandler("Listen: ", listen(listenSocket, SOMAXCONN));

        std::cout << "Proxy started on " << listenSocket << " port" << std::endl;

        while (true) {
            int clientSocket = accept(listenSocket, NULL, NULL);
            if (clientSocket == -1 && errno == EMFILE) {
                continue;
            } else if (clientSocket > 0) {
                workers->addTask(std::make_unique<ProxyTask>(
                    clientSocket, monitor, cache, workers, shutDown
                ));
            }
        }
    }

    void Proxy::shutdown() {
        workers->clearTasks();
    }

    Proxy::~Proxy() {
        close(listenSocket);
        shutdown();
        monitor.reset();
        cache.reset();
        workers.reset();
    }
}