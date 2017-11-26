#include "WelcomeView.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>

namespace phobos {

namespace {
QString const welcomeText =
R"(
<ul style="margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;">
  <li style="margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
    Import your photos
  </li>
  <ul style="list-style-type:none; margin-top: 0px; margin-bottom: 0px; margin-left: 10px; margin-right: 0px; -qt-list-indent: 1;">
    <li style=" margin-top:0px; margin-bottom:7px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - File > Open (Ctrl+O)
    </li>
  </ul>
  <li style="margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
    Select your favorites
  </li>
  <ul style="list-style-type:none; margin-top: 0px; margin-bottom: 0px; margin-left: 10px; margin-right: 0px; -qt-list-indent: 1;">
    <li style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - Arrow keys to move around
    </li>
    <li style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - View &gt; Next series (Shift+Right)
    </li>
    <li style=" margin-top:0px; margin-bottom:7px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - Enter to toggle selection
    </li>
  </ul>
  <li style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
    Automate you work
  </li>
  <ul style="list-style-type:none; margin-top: 0px; margin-bottom: 0px; margin-left: 10px; margin-right: 0px; -qt-list-indent: 1;">
    <li style=" margin-top:0px; margin-bottom:7px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - Action > Select best
    </li>
  </ul>
  <li style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
    Explore different views
  </li>
  <ul style="list-style-type:none; margin-top: 0px; margin-bottom: 0px; margin-left: 10px; margin-right: 0px; -qt-list-indent: 1;">
    <li style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - View > Separate photos (Alt+3)
    </li>
    <li style=" margin-top:0px; margin-bottom:7px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - View > Details (Ctrl+D)
    </li>
  </ul>
  <li style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
    Process your selection
  </li>
  <ul style="list-style-type:none; margin-top: 0px; margin-bottom: 0px; margin-left: 10px; margin-right: 0px; -qt-list-indent: 1;">
    <li style=" margin-top:0px; margin-bottom:7px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
      - Action > Move
    </li>
  </ul>
</ul>
)";
} // unnamed namespace

WelcomeView::WelcomeView(QWidget* parent) :
  QWidget(parent)
{
  QLabel* label = new QLabel(welcomeText);
  label->setStyleSheet("color:gray; font-size: 15px");

  QHBoxLayout* hLayout = new QHBoxLayout();
  hLayout->addStretch();
  hLayout->addWidget(label);
  hLayout->addStretch();

  QVBoxLayout* vLayout = new QVBoxLayout();
  vLayout->addStretch();
  vLayout->addLayout(hLayout);
  vLayout->addStretch();

  setLayout(vLayout);
}

} // namespace phobos
