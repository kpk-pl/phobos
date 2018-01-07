#ifndef IMAGEPROCESSING_FEATURE_HUE_H
#define IMAGEPROCESSING_FEATURE_HUE_H

#include <array>

namespace phobos { namespace iprocess { namespace feature {

struct Hue
{
  static constexpr std::size_t const numberOfChannels = 6;
  enum class Name : std::size_t
  {
    Red = 0, Yellow, Green, Cyan, Blue, Magenta
  };

  std::array<double, numberOfChannels> channel;
  double getChannel(Name const name) const
  {
    return channel[static_cast<std::size_t>(name)];
  }
};

}}} // namespace phobos::iprocess::feature

#endif // IMAGEPROCESSING_FEATURE_HUE_H
