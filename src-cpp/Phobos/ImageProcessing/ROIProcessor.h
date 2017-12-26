#ifndef IMAGEPROCESSING_ROI_PROCESSOR_H_
#define IMAGEPROCESSING_ROI_PROCESSOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <tuple>
#include <algorithm>

namespace phobos { namespace iprocess {

template<typename ResultT = double>
struct ROIProcessor
{
  template<typename Callable>
  std::vector<ResultT> operator()(cv::Mat const& image, Callable callable, std::size_t const N) const
  {
    auto const widthWide = image.cols / N;
    auto const heightWide = image.rows / N;
    auto const bins = N*N;

    std::vector<ResultT> result;
    result.reserve(bins);

    for (unsigned colPiece = 0; colPiece < N; ++colPiece)
      for (unsigned rowPiece = 0; rowPiece < N; ++rowPiece)
      {
        cv::Rect const roi(colPiece*widthWide, rowPiece*heightWide, widthWide, heightWide);
        result.emplace_back(callable(image(roi)));
      }

    return result;
  }
};

template<typename ResultT>
std::tuple<ResultT, ResultT, ResultT>
aggregateROI(std::vector<ResultT> roiResults)
{
  if (roiResults.empty())
    return std::make_tuple(ResultT{}, ResultT{}, ResultT{});

  if (roiResults.size() == 1)
    return std::make_tuple(roiResults[0], roiResults[0], roiResults[0]);

  std::sort(std::begin(roiResults), std::end(roiResults));

  return std::make_tuple(
    roiResults.front(),
    0.5 * (roiResults.at(roiResults.size() / 2) + roiResults.at((roiResults.size()+1) / 2)),
    roiResults.back());
}

}} // namespace phobos::iprocess

#endif // IMAGEPROCESSING_ROI_PROCESSOR_H_
