#ifndef WIDGETS_IMAGESHOWDIALOG_H
#define WIDGETS_IMAGESHOWDIALOG_H

#include "PhotoContainers/ItemId.h"
#include <QImage>

class QWidget;

namespace phobos { namespace widgets { namespace fulldialog {

void showImage(QWidget *parent, QImage const& image, pcontainer::ItemId const& photoItem);
void updateImage(QImage const& image, pcontainer::ItemId const& photoItem);
bool exists();

}}} // namespace phobos::widgets::fulldialog

#endif // WIDGETS_IMAGESHOWDIALOG_H
