#include <iostream>
#include <memory>
#include <csignal>
#include <unistd.h>
#include <string_view>

#include "util/error/error.hpp"
#include "proxy/proxy.hpp"

std::shared_ptr<CHP::Proxy> PROXY;
int PORT = 8080;
int THREADS = 4;
int CACHE_MIN_SIZE = 1;
int CACHE_MAX_SIZE = 10;
int CACHE_TTL = 5;

std::string_view HELP_INFO = 
"Proxy flags:\n"
"--help - information about proxy flags;\n"
"--port=<int> - the port on which the proxy will listen (default value is 8080);\n"
"--max-client-threads=<int> - the number of threads allocated for client processing (default value is 4);\n"
"--cache-initial-size=<int(MB)> - the initial size of the cache (default value is 1 MB);\n"
"--cache-max-size=<int(MB)> - the maximum size of the cache (default value is 10 MB);\n"
"--cache-ttl=<int(seconds)> - the maximum TTL of cache record (default value is 5 seconds);\n";

void handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Recieved SIGINT" << std::endl;
    } else if (signal == SIGTERM) {
        std::cout << "Recieved SIGTERM" << std::endl;
    }

    std::exit(0);
}

int initSignalHandlers() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::cerr << "Failed to set up SIGINT handler" << std::endl;
        return 1;
    }

    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        std::cerr << "Failed to set up SIGTERM handler" << std::endl;
        return 1;
    }

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if (sigprocmask(SIG_BLOCK, &set, nullptr) == -1) {
        std::cerr << "Failed to block SIGPIPE" << std::endl;
        return 1;
    }

    return 0;
}

int parseFlags(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "--help") {
            std::cout << HELP_INFO << std::endl;
            return 1;
        } else if (arg.starts_with("--port=")) {
            PORT = std::stoi(std::string(arg.substr(7)));
            if (PORT < 0 || PORT > 65535) {
                std::cerr << "Invalid port number" << std::endl;
                return 1;
            }
        } else if (arg.starts_with("--max-client-threads=")) {
            THREADS = std::stoi(std::string(arg.substr(18)));
            if (THREADS <= 0) {
                std::cerr << "Invalid number of threads" << std::endl;
                return 1;
            }
        } else if (arg.starts_with("--cache-initial-size=")) {
            CACHE_MIN_SIZE = std::stoi(std::string(arg.substr(17)));
            if (CACHE_MIN_SIZE <= 0) {
                std::cerr << "Invalid initial cache size" << std::endl;
                return 1;
            }
        } else if (arg.starts_with("--cache-max-size=")) {
            CACHE_MAX_SIZE = std::stoi(std::string(arg.substr(16)));
            if (CACHE_MAX_SIZE <= 0) {
                std::cerr << "Invalid maximum cache size" << std::endl;
                return 1;
            }
        } else if (arg.starts_with("--cache-ttl=")) {
            CACHE_TTL = std::stoi(std::string(arg.substr(13)));
            if (CACHE_TTL <= 0) {
                std::cerr << "Invalid cache TTL" << std::endl;
                return 1;
            }
        } else {
            std::cout << "Unknown flag: " << arg << std::endl;
            std::cout << HELP_INFO << std::endl;
        }
    }

    if (CACHE_MIN_SIZE > CACHE_MAX_SIZE) {
        std::cerr << "Invalid cache size range" << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (parseFlags(argc, argv)) {
        return 0;
    }

    if (initSignalHandlers()) {
        return 1;
    }

    try {
        PROXY.reset(new CHP::Proxy(PORT, THREADS, CACHE_MIN_SIZE, CACHE_MAX_SIZE, CACHE_TTL));
        PROXY->start();
    } catch(util::Exception& e) {
        std::cerr << "Error: " << e.getMessage() << std::endl;
        return 1;
    }
    return 0;
}
