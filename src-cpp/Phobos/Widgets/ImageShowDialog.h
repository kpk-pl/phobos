#ifndef WIDGETS_IMAGESHOWDIALOG_H
#define WIDGETS_IMAGESHOWDIALOG_H

#include "PhotoContainers/Fwd.h"
#include <QImage>

class QWidget;

namespace phobos { namespace widgets { namespace fulldialog {

void showImage(QWidget *parent, QImage const& image, pcontainer::Item const& photoItem);
bool exists();

}}} // namespace phobos::widgets::fulldialog

#endif // WIDGETS_IMAGESHOWDIALOG_H
