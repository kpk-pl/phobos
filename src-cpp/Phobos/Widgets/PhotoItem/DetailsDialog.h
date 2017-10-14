#ifndef WIDGETS_PHOTOITEM_DETAILS_DIALOG_H
#define WIDGETS_PHOTOITEM_DETAILS_DIALOG_H

#include "PhotoContainers/Fwd.h"
#include "ImageProcessing/MetricsFwd.h"

class QImage;
class QWidget;

namespace phobos { namespace widgets { namespace pitem {

void showDetailsDialog(QWidget *parent,
                       pcontainer::Item const& photoItem,
                       QImage const& image,
                       iprocess::metric::MetricPtr const& metrics);

}}} // namespace phobos::widgets::pitem

#endif // WIDGETS_PHOTOITEM_DETAILS_DIALOG_H
