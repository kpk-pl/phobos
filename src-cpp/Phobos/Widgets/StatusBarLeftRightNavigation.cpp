#include "Widgets/StatusBarLeftRightNavigation.h"
#include "Widgets/StatusBarButton.h"
#include <QHBoxLayout>

namespace phobos { namespace widgets {

StatusBarLeftRightNavigation::StatusBarLeftRightNavigation(config::ConfigPath const& configPath) :
  leftButton(new StatusBarButton(configPath("leftButton"))),
  rightButton(new StatusBarButton(configPath("rightButton")))
{
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(leftButton);
  layout->addWidget(rightButton);

  setLayout(layout);

  leftButton->setToolTip(tr("Previous photo"));
  rightButton->setToolTip(tr("Next photo"));

  QObject::connect(leftButton, &QPushButton::clicked, this, &StatusBarLeftRightNavigation::leftClicked);
  QObject::connect(rightButton, &QPushButton::clicked, this, &StatusBarLeftRightNavigation::rightClicked);
}

}} // namespace phobos::widgets
