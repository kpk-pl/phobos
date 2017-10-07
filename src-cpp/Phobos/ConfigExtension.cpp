#include "ConfigExtension.h"
#include "Config.h"
#include <cctype>

namespace phobos { namespace config {

cpptoml::option<QSize> qSize(std::string const& qualifiedPath)
{
  cpptoml::option<QSize> result;

  try
  {
    auto const table = get()->get_qualified(qualifiedPath);
    if (!table)
      return result;

    if (table->is_array())
    {
      auto const& arr = table->as_array()->get();
      if (arr.size() == 2)
      {
        auto const width = arr[0]->as<int64_t>();
        auto const height = arr[1]->as<int64_t>();
        if (width && width->get() >= 0 && height && height->get() >= 0)
          result = QSize(width->get(), height->get());
      }
    }
    else
    {
      auto const dict = table->as_table();
      auto const width = dict->get_as<std::size_t>("width");
      auto const height = dict->get_as<std::size_t>("height");
      if (width && height)
        result = QSize(*width, *height);
    }
  }
  catch (std::out_of_range const&)
  {}

  return result;
}

QSize qSize(std::string const& qualifiedPath, QSize const& def)
{
    return qSize(qualifiedPath).value_or(def);
}

cpptoml::option<QColor> qColor(std::string const& qualifiedPath)
{
    cpptoml::option<QColor> result;

    auto const value = get()->get_qualified_as<std::string>(qualifiedPath);
    if (!value)
        return result;

    QColor color(value->c_str());
    if (color.isValid())
        result = color;

    return result;
}

QColor qColor(std::string const& qualifiedPath, QColor const& def)
{
    return qColor(qualifiedPath).value_or(def);
}

QFont qFont(std::string const& qualifiedPath)
{
    QFont result;

    auto const table = get()->get_table_qualified(qualifiedPath);
    if (!table)
        return result;

    auto const family = table->get_as<std::string>("family");
    if (family)
        result.setFamily(family->c_str());

    auto const pointSize = table->get_as<unsigned>("pointSize");
    if (pointSize)
        result.setPointSize(*pointSize);

    auto const weight = table->get_as<unsigned>("weight");
    if (weight)
        result.setWeight(*weight);

    return result;
}

cpptoml::option<std::size_t> bytes(std::string const& qualifiedPath)
{
  auto const value = get()->get_qualified_as<std::string>(qualifiedPath);
  if (!value || value->empty())
    return cpptoml::option<std::size_t>{};

  switch(std::toupper(value->at(value->size()-1)))
  {
    case 'K':
      return static_cast<std::size_t>(std::stod(value->substr(0, value->size()-1)) * 1024.0);
    case 'M':
      return static_cast<std::size_t>(std::stod(value->substr(0, value->size()-1)) * 1024.0 * 1024.0);
    case 'G':
      return static_cast<std::size_t>(std::stod(value->substr(0, value->size()-1)) * 1024.0 * 1024.0 * 1024.0);
    case 'T':
      return static_cast<std::size_t>(std::stod(value->substr(0, value->size()-1)) * 1024.0 * 1024.0 * 1024.0 * 1024.0);
    default:
      return std::stoull(*value);
  }
}

std::size_t bytes(std::string const& qualifiedPath, std::size_t const def)
{
  return bytes(qualifiedPath).value_or(def);
}

}} // namespace phobos::config
