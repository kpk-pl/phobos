#ifndef CONFIGPATH_H
#define CONFIGPATH_H

#include <string>

namespace phobos { namespace config {

class ConfigPath
{
public:
  ConfigPath() = default;
  ConfigPath(std::string const& path);

  operator std::string const&() const&;
  operator std::string() const;

  ConfigPath operator+(std::string const& rhs) const;
  ConfigPath operator+(const char* rhs) const;

  ConfigPath operator()(std::string const& subPath) const;
  ConfigPath operator()(const char* subPath) const;

  void operator+=(std::string const& subPath);
  void operator+=(const char* subPath);

private:
  std::string path;
};

}} // namespace phobos::config

#endif // CONFIGPATH_H
