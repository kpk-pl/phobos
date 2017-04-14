#ifndef CPPTOML_EXTENSIONS_H
#define CPPTOML_EXTENSIONS_H

#include <string>
#include <QSize>
#include <QColor>
#include <cpptoml/cpptoml.h>
#include "Config.h"

namespace phobos { namespace config {

template<typename T>
T qualified(std::string const& qualifiedPath, T const& def)
{
    return get()->get_qualified_as<T>(qualifiedPath).value_or(def);
}

cpptoml::option<QSize> qSize(std::string const& qualifiedPath);
QSize qSize(std::string const& qualifiedPath, QSize const& def);

cpptoml::option<QColor> qColor(std::string const& qualifiedPath);
QColor qColor(std::string const& qualifiedPath, QColor const& def);

}} // namespace phobos::config

#endif // CPPTOML_EXTENSIONS_H
