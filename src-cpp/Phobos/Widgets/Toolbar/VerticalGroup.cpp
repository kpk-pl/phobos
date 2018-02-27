#include "Widgets/Toolbar/VerticalGroup.h"
#include <QVBoxLayout>

namespace phobos { namespace widgets { namespace toolbar {

VerticalGroup::VerticalGroup()
{
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0, 2, 0, 0);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  setLayout(layout);
}

void VerticalGroup::addWidget(QWidget *widget)
{
  layout()->addWidget(widget);
}

}}} // namespace phobos::widgets::toolbar
