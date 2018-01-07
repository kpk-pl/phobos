#ifndef WIDGETS_IMAGEWIDGET_H
#define WIDGETS_IMAGEWIDGET_H

#include "Widgets/HeightResizeableInterface.h"
#include "Widgets/ClickableLabel.h"
#include "ImageProcessing/MetricsFwd.h"
#include "Utils/Soul.h"
#include <QImage>
#include <QSize>
#include <memory>

namespace phobos { namespace widgets {

class ImageWidget : public ClickableLabel, public HeightResizeableInterface
{
  Q_OBJECT

public:
  explicit ImageWidget(QSize const suggestedSize);
  explicit ImageWidget(QImage const& image);

  QImage image() const { return _image; }
  iprocess::MetricPtr metrics() const { return _metrics; }

  void setBorder(std::size_t const width);

  int heightForWidth(const int width) const override;
  int widthForHeight(const int height) const override;

  bool hasHeightForWidth() const override;
  bool hasWidthForHeight() const override;

  void paintEvent(QPaintEvent* event) override;

  utils::Soul<ImageWidget>::Lifetime lifetime() const { return soul.lifetime(); }

public slots:
  void setImage(QImage const& image);
  void setMetrics(iprocess::MetricPtr metrics);

private:
  QSize _size;
  QImage _image;
  int borderWidth = 0;
  iprocess::MetricPtr _metrics;
  utils::Soul<ImageWidget> const soul;
};

}} // namespace phobos::widgets

#endif // WIDGETS_IMAGEWIDGET_H
