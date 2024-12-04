#pragma once

#include <string>
#include <cstring>
#include <errno.h>

namespace util {
    class Exception {
    public:
        Exception(const std::string& message): message(message) {}
        const std::string& getMessage() const { return message; }
    private:
        std::string message;
    };

    int cErrHandler(const std::string& additionalInfo, int resultOfOperation);
} // namespace util