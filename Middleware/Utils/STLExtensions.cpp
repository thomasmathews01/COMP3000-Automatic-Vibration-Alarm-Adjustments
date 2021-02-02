#include "STLExtensions.h"

namespace STLExtensions {
    std::optional<int> opt_stoi(const std::string& string) {
        try {
            const auto string_as_int = std::stoi(string);

            return string_as_int;
        }
        catch (const std::exception& exception) {
            return std::nullopt;
        }
    }
}