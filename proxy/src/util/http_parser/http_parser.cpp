#include "http_parser.hpp"

#include <string>

namespace util::http {
    namespace response {
        std::string_view getVersion(std::string_view response) {
            size_t start = response.find('/') + 1;
            size_t end = response.find(' ');
            return std::string_view(response.begin() + start, end - start);
        }

        int getStatus(std::string_view response) {
            size_t start = response.find(' ') + 1;
            size_t end = response.find(' ', start);
            return std::stoi(std::string(response.begin() + start, end - start));
        }

        std::string_view getStatusExplain(std::string_view response) {
            size_t end = response.find("\r\n");
            size_t start = response.rfind(' ', end) + 1;
            return std::string_view(response.begin() + start, end - start);
        }
    }

    namespace request {
        std::string_view getMethod(std::string_view request) {
            return std::string_view(request.begin(), request.find(' '));
        }

        std::string_view getUri(std::string_view request) {
            size_t start = request.find(' ') + 1;
            size_t end = request.find(' ', start);
            return std::string_view(request.begin() + start, end - start);
        }
    
        std::string_view getVersion(std::string_view request) {
            size_t end = request.find("\r\n");

            if (request.find(' ') == request.rfind(' ', end)) {
                return "0.9";
            }

            size_t start = request.rfind("/", end) + 1;

            return std::string_view(request.begin() + start, end - start);
        }
    }

    std::vector<std::pair<std::string_view, std::string_view>> getHeaders(std::string_view message) {
        std::vector<std::pair<std::string_view, std::string_view>> headers;
        size_t start = message.find("\r\n") + 2;

        while (true) {
            size_t end = message.find("\r\n", start);
            if (start == end) {
                break;
            }

            size_t colon = message.find(':', start);
            headers.push_back(
                {
                    std::string_view(message.begin() + start, colon - start), 
                    std::string_view(message.begin() + colon + 2, end - colon - 2)
                }
            );

            start = end + 2;
            if (start == message.npos) {
                break;
            }
        }

        return headers;
    }
            
    std::optional<std::string_view> getBody(std::string_view message) {
        size_t start = message.find("\r\n\r\n");
        if (start == message.npos) {
            return std::nullopt;
        }

        start += 4;
        if (start >= message.size()) {
            return std::nullopt;
        }
        return std::string_view(message.begin() + start, message.size() - start);
    }
}