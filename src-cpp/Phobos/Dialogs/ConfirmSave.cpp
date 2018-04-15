#include "Dialogs/ConfirmSave.h"

#include <QGridLayout>
#include <QStyle>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QPixmap>
#include <QSpacerItem>

namespace phobos { namespace dialog {

ConfirmSave::ConfirmSave(QString const& filename, QWidget *parent) :
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), pDontAskAgain(false)
{
  setWindowTitle(tr("Confirm save"));

  QGridLayout *lt = new QGridLayout;

  QLabel *icon = new QLabel();
  icon->setObjectName(QLatin1String("qt_msgboxex_icon_label"));
  icon->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(QSize(32, 32)));
  icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QLabel *label = new QLabel(tr("Do you want to replace existing file?"));
  label->setObjectName(QLatin1String("qt_msgbox_label"));
  label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

  QLabel *fileNameLabel = new QLabel(filename);
  fileNameLabel->setObjectName(QLatin1String("qt_msgbox_label"));
  fileNameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

  QSpacerItem *spacer = new QSpacerItem(7, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);

  lt->addWidget(icon, 0, 0, 2, 1, Qt::AlignTop);
  lt->addItem(spacer, 0, 1, 2, 1);
  lt->addWidget(label, 0, 2, 1, 1);
  lt->addWidget(fileNameLabel, 1, 2, 1, 1);

  QCheckBox *chBox = new QCheckBox(tr("Do not ask me again"));
  QObject::connect(chBox, &QCheckBox::stateChanged,
                   [this](int const state){ pDontAskAgain = (state == Qt::Checked); });

  lt->addWidget(chBox, 2, 0, 1, 3, Qt::AlignLeft);

  lt->addItem(new QSpacerItem(1, 7, QSizePolicy::Fixed, QSizePolicy::Fixed), 3, 0);

  QPushButton *yesButton = new QPushButton(tr("Yes"));
  QPushButton *noButton = new QPushButton(tr("No"));

  QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->setObjectName(QLatin1String("qt_msgbox_buttonbox"));
  buttonBox->setCenterButtons(style()->styleHint(QStyle::SH_MessageBox_CenterButtons, 0, this));
  buttonBox->addButton(yesButton, QDialogButtonBox::AcceptRole);
  buttonBox->addButton(noButton, QDialogButtonBox::RejectRole);
  noButton->setDefault(true);

  QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &ConfirmSave::accept);
  QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &ConfirmSave::reject);

  lt->addWidget(buttonBox, 4, 0, 1, 3);

  setLayout(lt);
}

}} // namespace phobos::dialog
