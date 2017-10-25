#include "Widgets/PhotoItem/AddonRenderer.h"
#include "ImageProcessing/Metrics.h"
#include "PhotoContainers/Item.h"
#include "ConfigExtension.h"
#include "Utils/Asserted.h"
#include "Utils/Algorithm.h"
#include <QPainter>
#include <QSize>
#include <QPoint>
#include <algorithm>

namespace phobos { namespace widgets { namespace pitem {

AddonRenderer::AddonRenderer(QPainter &painter, config::ConfigPath const& baseConfigPath) :
  painter(painter), baseConfigPath(baseConfigPath)
{}

namespace {
struct PainterCtx
{
  PainterCtx(QPainter &qPainter) : painter(qPainter)
  {
    painter.save();
  }
  ~PainterCtx()
  {
    painter.restore();
  }
private:
  QPainter &painter;
};

QSize histogramSize(iprocess::metric::Histogram const& hist, QSize size)
{
  static std::vector<std::size_t> const POW2 = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

  std::size_t maxHistSize = 0;
  for (auto const& data : hist.data)
  {
    assert(utils::valueIn(data.second.size(), POW2));
    maxHistSize = std::max(maxHistSize, data.second.size());
  }

  for (auto it = POW2.rbegin(); it != POW2.rend(); ++it)
    if (size.width() >= static_cast<int>(*it))
    {
      size.setWidth(*it);
      break;
    }

  if (size.width() > static_cast<int>(maxHistSize))
    size.setWidth(maxHistSize);

  return size;
}

std::vector<float> scaleHistogram(std::vector<float> const& data, std::size_t const width)
{
  if (data.empty()) return data;

  unsigned const mult = data.size()/width;
  std::vector<float> accumulated;
  accumulated.reserve(width);

  for (std::size_t i = 0; i < width; ++i)
    accumulated.push_back(std::accumulate(data.begin()+i*mult, data.begin()+(i+1)*mult, 0.0));

  double const maxBin = *std::max_element(accumulated.begin(), accumulated.end());
  std::transform(accumulated.begin(), accumulated.end(), accumulated.begin(),
      [maxBin](double const v){ return v/maxBin; });

  return accumulated;
}

} // unnamed namespace

void AddonRenderer::histogram(iprocess::metric::Histogram const& hist, QSize const& prefferedSize, QPoint const& origin)
{
  auto const histConfig = baseConfigPath("histogram");
  QSize const histSize = histogramSize(hist, prefferedSize);

  using Channel = iprocess::metric::Histogram::Channel;

  drawHistLike(utils::asserted::fromMap(hist.data, Channel::Value), histConfig("value"), origin, histSize);
  drawHistLike(utils::asserted::fromMap(hist.data, Channel::Blue), histConfig("blue"), origin, histSize);
  drawHistLike(utils::asserted::fromMap(hist.data, Channel::Green), histConfig("green"), origin, histSize);
  drawHistLike(utils::asserted::fromMap(hist.data, Channel::Red), histConfig("red"), origin, histSize);
}

void AddonRenderer::cumulativeHistogram(iprocess::metric::Histogram const& hist, QSize const& prefferedSize, QPoint const& origin)
{
  auto const histConfig = baseConfigPath("cumulativeHistogram");
  QSize const histSize = histogramSize(hist, prefferedSize);

  auto const cumulate = [](std::vector<float> const& data){
    std::vector<float> result;
    result.reserve(data.size());
    std::partial_sum(data.begin(), data.end(), std::back_inserter(result));
    return result;
  };

  using Channel = iprocess::metric::Histogram::Channel;

  drawHistLike(cumulate(utils::asserted::fromMap(hist.data, Channel::Value)), histConfig("value"), origin, histSize);
  drawHistLike(cumulate(utils::asserted::fromMap(hist.data, Channel::Blue)), histConfig("blue"), origin, histSize);
  drawHistLike(cumulate(utils::asserted::fromMap(hist.data, Channel::Green)), histConfig("green"), origin, histSize);
  drawHistLike(cumulate(utils::asserted::fromMap(hist.data, Channel::Red)), histConfig("red"), origin, histSize);
}

void AddonRenderer::drawHistLike(std::vector<float> const& data,
                                 config::ConfigPath const& configPath,
                                 QPoint const& origin,
                                 QSize const& size)
{
  PainterCtx ctx(painter);

  painter.setPen(config::qColor(configPath("color"), Qt::black));
  painter.setOpacity(config::qualified(configPath("opacity"), 1.0));

  auto const scaledHist = scaleHistogram(data, size.width());
  std::string const& type = config::qualified<std::string>(configPath("style"), "none");
  double const H = size.height();

  if (type == "fill")
    for (std::size_t i = 0; i < scaledHist.size(); ++i)
      painter.drawLine(origin.x() + i, origin.y() + H,
                       origin.x() + i, origin.y() + (1.0-scaledHist[i])*H);
  else if (type == "line")
    for (std::size_t i = 1; i < scaledHist.size(); ++i)
      painter.drawLine(origin.x() + (i-1), origin.y() + (1.0-scaledHist[i-1])*H,
                       origin.x() + i,     origin.y() + (1.0-scaledHist[i])*H);

}

void AddonRenderer::hueDisplay(iprocess::metric::Hue const& data,
                               QSize const& prefferedSize,
                               QPoint const& origin)
{
  static_assert(iprocess::metric::Hue::numberOfChannels == 6, "Only 6 channels implemented");
  PainterCtx ctx(painter);

  auto const hueConfig = baseConfigPath("hueDisplay");
  std::size_t const barSpacing = config::qualified(hueConfig("barSpacing"), 3u);
  std::size_t const barHeight = (prefferedSize.height() - (6-1)*barSpacing) / 6;

  QColor const colors[6] = {QColor(255, 0, 0), QColor(255, 255, 0), QColor(0, 255, 0),
                            QColor(0, 255, 255), QColor(0, 0, 255), QColor(255, 0, 255)};


  for (std::size_t ch = 0; ch < 6; ++ch)
  {
    QRect const barRect(origin.x() + prefferedSize.width()*(1.0 - data.channel[ch]),
                        origin.y() + ch*(barHeight+barSpacing),
                        prefferedSize.width() * data.channel[ch],
                        barHeight);

    painter.setPen(colors[ch]);
    painter.setBrush(colors[ch]);
    painter.drawRect(barRect);
  }

  QFont font = painter.font();
  font.setPixelSize(barHeight);
  painter.setFont(font);
  painter.setPen(Qt::black);

  for (std::size_t ch = 0; ch < 6; ++ch)
  {
    painter.drawText(origin.x(), origin.y() + ch*(barHeight+barSpacing),
                     prefferedSize.width(), barHeight+1, // +1 is a hack for Linux, which ignores the correct setting
                     Qt::AlignLeft | Qt::TextSingleLine,
                     QString::number(data.channel[ch]*100.0, 'f', 1) + " %");
  }
}

}}} // namespace phobos::widgets::pitem
