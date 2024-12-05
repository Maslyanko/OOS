#include "proxy_task.hpp"
#include "../util/http_parser/http_parser.hpp"

#include <string>
#include <string_view>
#include <cstring>
#include <memory>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

namespace CHP {
    int ProxyTask::readFrom(int fd, std::shared_ptr<char[]>& buffer) {
        buffer = std::make_shared<char[]>(startBufferSize);
        int bufferSize = startBufferSize;
        int cursor = 0;

        while (true) {
            int result = read(fd, buffer.get() + cursor, bufferSize - cursor);
            if (result == -1) {
                return -1;
            }

            cursor += result;
            if ((cursor < bufferSize) || result == 0) {
                break;
            }

            if (cursor == bufferSize) {
                int newSize = 2 * bufferSize;
                char* newBuffer = new char[newSize];

                std::memcpy(newBuffer, buffer.get(), bufferSize);
                buffer.reset(newBuffer);

                bufferSize = newSize;
            }
        }

        return cursor;
    }

    int ProxyTask::writeTo(int fd, std::shared_ptr<char[]>& buffer, int length) {
        int cursor = 0;
        while (cursor < length) {
            int toWrite = length - cursor < startBufferSize? length - cursor: startBufferSize;
            int result = write(fd, buffer.get() + cursor, toWrite);
            if (result == -1) {
                return -1;
            }

            cursor += result;
        }

        return 0;
    }

    int ProxyTask::connectToServer(std::string_view address, int port) {
        struct addrinfo hints, *result = nullptr;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(std::string(address).c_str(), std::to_string(port).c_str(), &hints, &result) != 0) {
            if (result) {
                freeaddrinfo(result);
            }
            return -1;
        }

        int tmp = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (tmp == -1) {
            freeaddrinfo(result);
            return -1;
        }

        if (connect(tmp, result->ai_addr, result->ai_addrlen) == -1) {
            freeaddrinfo(result);
            return -1;
        }

        freeaddrinfo(result);
        return tmp;
    }

    ProxyTask::ProxyTask(
        int client, 
        std::shared_ptr<RequestMonitor>& monitor,
        std::shared_ptr<cache::Cache>& cache,
        std::shared_ptr<MT::ThreadPool>& workers,
        bool& shutdown
    ): shutdown(shutdown) {
        this->client = client;
        this->monitor = monitor;
        this->cache = cache;
        this->workers = workers;
    }

    void ProxyTask::execute() {
        std::shared_ptr<char[]> request;
        int requestLength = readFrom(client, request);
        if (requestLength == -1) {
            return;
        }

        auto method = util::http::request::getMethod(request.get());
        if (method != "GET" && method != "HEAD" && method != "POST") {
            return;
        }

        auto cacheEntry = cache.get()->get(request, requestLength);
        std::shared_ptr<char[]> response = cacheEntry.first;
        int responseLength = cacheEntry.second;

        bool communicateServer = false;

        if (!response && !monitor->registration(request.get())) {
            monitor->wait(request.get());

            cacheEntry = cache.get()->get(request, requestLength);
            response = cacheEntry.first;
            if (!response) {
                communicateServer = true;
            }

            responseLength = cacheEntry.second;
        }

        if (communicateServer) {
            monitor->registration(std::string_view(request.get(), requestLength));

            auto headers = util::http::getHeaders(request.get());
            std::string_view host;
            for (const auto& header : headers) {
                if (header.first == "Host") {
                    host = header.second;
                    break;
                }
            }

            while (true) {
                server = connectToServer(host, 80);
                if (server == -1) {
                    break;
                }

                if (writeTo(server, request, requestLength)) {
                    break;
                }

                responseLength = readFrom(server, response);
                if (responseLength == -1) {
                    break;
                }

                int status = util::http::response::getStatus(response.get());

                if (method == "GET" && status >= 200 && status < 300) {
                    cache->insert(request, requestLength, response, responseLength);
                    break;
                } else if (status > 300 && status < 400) {
                    headers = util::http::getHeaders(response.get());
                    host = "";
                    for (auto& header : headers) {
                        if (header.first == "Location") {
                            host = header.second;
                            break;
                        }
                    }
                } else {
                    break;
                }
            }

            monitor->notify(request.get());
        }

        if (responseLength > 0) {
            writeTo(client, response, responseLength);
        }
    }

    ProxyTask::~ProxyTask() {
        shutdown = true;
        close(client);
        if (server > 0) {
            close(server);
        }
    }
}