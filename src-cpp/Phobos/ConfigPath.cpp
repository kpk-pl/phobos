#include "ConfigPath.h"

namespace phobos { namespace config {

ConfigPath::ConfigPath(std::string const& path) : path(path)
{}

ConfigPath::operator std::string const&() const&
{
  return path;
}

ConfigPath::operator std::string() const
{
  return path;
}

std::string const& ConfigPath::toString() const
{
  return path;
}

ConfigPath ConfigPath::operator+(std::string const& rhs) const
{
  ConfigPath result = *this;
  result += rhs;
  return result;
}

ConfigPath ConfigPath::operator+(const char* rhs) const
{
  ConfigPath result = *this;
  result += rhs;
  return result;
}

ConfigPath ConfigPath::operator()(std::string const& subPath) const
{
  return *this + subPath;
}

ConfigPath ConfigPath::operator()(const char* subPath) const
{
  return *this + subPath;
}

void ConfigPath::operator+=(std::string const& subPath)
{
  if (!path.empty())
    path += '.';
  path += subPath;
}

void ConfigPath::operator+=(const char* subPath)
{
  if (*subPath != '\0')
    path += '.';
  path += subPath;
}

}} // namespace phobos::config
