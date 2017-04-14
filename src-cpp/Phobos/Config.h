#ifndef CONFIG_H
#define CONFIG_H

#include <memory>
#include <cpptoml/cpptoml.h>

namespace phobos { namespace config {

std::shared_ptr<cpptoml::table> const& get();

}} // namespace phobos::config

#endif // CONFIG_H
