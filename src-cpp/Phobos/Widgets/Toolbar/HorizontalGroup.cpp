#include "Widgets/Toolbar/HorizontalGroup.h"
#include "Widgets/Layout/Raw.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

namespace phobos { namespace widgets { namespace toolbar {

namespace detail {
HorizontalGroupBase::HorizontalGroupBase() :
  _buttonLayout(nullptr), _buddy(nullptr)
{}

void HorizontalGroupBase::addWidget(QWidget *widget)
{
  QVBoxLayout *lt = widgets::layout::makeRaw<QVBoxLayout>();
  lt->addWidget(widget, 1);
  lt->addStretch();
  _buttonLayout->addLayout(lt);
}

void HorizontalGroupBase::setVisible(bool visible)
{
  QWidget::setVisible(visible);
  if (_buddy)
    _buddy->setVisible(visible);
}

void HorizontalGroupBase::setVisibleBuddy(QWidget *buddy)
{
  _buddy = buddy;
}
} // namespace detail

HorizontalGroup::HorizontalGroup()
{
  _buttonLayout = new QHBoxLayout;
  _buttonLayout->setContentsMargins(0, 0, 0, 0);
  _buttonLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  setLayout(_buttonLayout);
}

namespace {
class GroupLabel : public QLabel
{
public:
  explicit GroupLabel(QString const& label, QWidget *parent = nullptr) : QLabel(label, parent)
  {
    setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  }
};
} // unnamed namespace

NamedHorizontalGroup::NamedHorizontalGroup(QString const& name)
{
  _buttonLayout = new QHBoxLayout;
  _buttonLayout->setContentsMargins(0, 0, 0, 0);
  _buttonLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

  QVBoxLayout *group = widgets::layout::makeRaw<QVBoxLayout>();
  group->addLayout(_buttonLayout);
  group->addWidget(new GroupLabel(name));
  setLayout(group);
}

}}} // namespace phobos::widgets::toolbar
