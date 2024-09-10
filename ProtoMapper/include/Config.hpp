#ifndef PROTOMAPPER_CONFIG_HPP
#define PROTOMAPPER_CONFIG_HPP

#include <string>

namespace proto
{
    [[nodiscard]] constexpr std::string GetTextDir() { return "/config/data/text"; }
    [[nodiscard]] constexpr std::string GetProfileDir() { return "/config/profile/"; }
    [[nodiscard]] constexpr std::string GetUIDir() { return "/config/ui/"; }
    [[nodiscard]] constexpr std::string GetAssetDir() { return "/assets"; }
}

#endif // !PROTOMAPPER_CONFIG_HPP
