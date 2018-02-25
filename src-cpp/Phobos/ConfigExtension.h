#ifndef CPPTOML_EXTENSIONS_H
#define CPPTOML_EXTENSIONS_H

#include "Config.h"
#include <cpptoml/cpptoml.h>
#include <boost/optional.hpp>
#include <QSize>
#include <QColor>
#include <QFont>
#include <string>
#include <vector>
#include <set>

namespace phobos { namespace config {

namespace detail {
template<typename T>
struct Retrieve
{
  cpptoml::option<T> operator()(std::string const& qualifiedPath) const
  {
    return get()->get_qualified_as<T>(qualifiedPath);
  }
};
template<typename T>
struct Retrieve<std::vector<T>>
{
  cpptoml::option<std::vector<T>> operator()(std::string const& qualifiedPath) const
  {
    return get()->get_qualified_array_of<T>(qualifiedPath);
  }
};
template<typename T>
struct Retrieve<std::set<T>>
{
  cpptoml::option<std::set<T>> operator()(std::string const& qualifiedPath) const
  {
    auto vec = get()->get_qualified_array_of<T>(qualifiedPath);
    if (vec)
      return std::set<T>(vec->begin(), vec->end());
    return {};
  }
};
} // namespace detail

template<typename T>
T qualified(std::string const& qualifiedPath, T const& def)
{
  return detail::Retrieve<T>{}(qualifiedPath).value_or(def);
}

template<typename T>
boost::optional<T> qualified(std::string const& qualifiedPath)
{
  auto const v = detail::Retrieve<T>{}(qualifiedPath);
  return v ? boost::optional<T>(*v) : boost::none;
}

cpptoml::option<QSize> qSize(std::string const& qualifiedPath);
QSize qSize(std::string const& qualifiedPath, QSize const& def);

cpptoml::option<QColor> qColor(std::string const& qualifiedPath);
QColor qColor(std::string const& qualifiedPath, QColor const& def);

QFont qFont(std::string const& qualifiedPath);

cpptoml::option<std::size_t> bytes(std::string const& qualifiedPath);
std::size_t bytes(const std::string &qualifiedPath, std::size_t const def);

}} // namespace phobos::config

#endif // CPPTOML_EXTENSIONS_H
