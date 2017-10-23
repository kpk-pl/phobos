#include "Utils/ExifReader.h"
#include <fstream>
#include <vector>

namespace phobos { namespace utils {

namespace {
std::vector<std::uint8_t> readBinary(std::string const& fileName)
{
  std::vector<std::uint8_t> data;

  std::ifstream ss(fileName, std::ios::in | std::ios::binary);
  if (!ss)
    return data;

  ss.seekg(0, std::ios::end);
  auto const length = ss.tellg();
  ss.seekg(0, std::ios::beg);

  data.resize(length);
  ss.read(reinterpret_cast<char*>(data.data()), length);

  return data;
}
} // unnamed namespace

boost::optional<easyexif::EXIFInfo> readExif(std::string const& fileName)
{
  auto const data = readBinary(fileName);
  if (data.empty())
    return boost::none;

  easyexif::EXIFInfo result;
  int const code = result.parseFrom(data.data(), data.size());
  if (code)
    return boost::none;

  return std::move(result);
}

}} // namespace phobos::utils
