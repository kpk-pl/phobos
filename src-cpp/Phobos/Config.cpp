#include "Config.h"

namespace phobos { namespace config {

std::shared_ptr<cpptoml::table> const& get()
{
    static auto toml = cpptoml::parse_file("config.toml");
    return toml;
}

}} // namespace phobos::config
