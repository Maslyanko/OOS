#include "error.hpp"

namespace util {
    int cErrHandler(const std::string& additionalInfo, int resultOfOperation) {
        if (resultOfOperation == -1) {
            throw Exception(additionalInfo + strerror(errno));
        }
        return resultOfOperation;
    }
}