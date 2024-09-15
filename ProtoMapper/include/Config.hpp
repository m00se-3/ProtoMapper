#ifndef PROTOMAPPER_CONFIG_HPP
#define PROTOMAPPER_CONFIG_HPP

#include <string>

namespace proto
{
    [[nodiscard]] constexpr std::string GetTextDir() { return TEXT_DIR; }
    [[nodiscard]] constexpr std::string GetProfileDir() { return PROFILE_DIR; }
    [[nodiscard]] constexpr std::string GetUIDir() { return UI_DIR; }
    [[nodiscard]] constexpr std::string GetAssetDir() { return ASSETS_DIR; }
}

#endif // !PROTOMAPPER_CONFIG_HPP
