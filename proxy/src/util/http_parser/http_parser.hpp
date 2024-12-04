#pragma once

#include <string_view>
#include <vector>
#include <optional>

namespace util {
    namespace http {
        namespace response {
            std::string_view getVersion(std::string_view response);
            int getStatus(std::string_view response);
            std::string_view getStatusExplain(std::string_view response);
        }

        namespace request {
            std::string_view getMethod(std::string_view request);
            std::string_view getUri(std::string_view request);
            std::string_view getVersion(std::string_view request);
        }

        std::vector<std::pair<std::string_view, std::string_view>> getHeaders(std::string_view request);
        std::optional<std::string_view> getBody(std::string_view request);
    }
} // namespace util