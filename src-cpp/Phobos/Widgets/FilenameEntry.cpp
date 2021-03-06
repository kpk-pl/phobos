#include "Widgets/FilenameEntry.h"
#include "Widgets/IconLabel.h"
#include "Widgets/ClickableLabel.h"
#include "Utils/Filesystem/Portable.h"
#include <QValidator>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>

namespace phobos { namespace widgets {

namespace {
class FileNameValidator : public QValidator
{
public:
  using QValidator::QValidator;

  QValidator::State validate(QString &input, int&) const override
  {
    if (input.isEmpty())
      return QValidator::State::Acceptable;

    std::string s = input.toStdString();

    // Replace '%.' sequences with acceptable input characters
    for (std::size_t pos = 0; pos < s.length(); ++pos)
      if (s[pos] == '%')
        s.replace(pos, 2, 2, '_');

    return utils::fs::portableFileName(s) ? QValidator::State::Acceptable : QValidator::State::Invalid;
  }
};

std::string sortCharacters(std::string str)
{
  std::sort(str.begin(), str.end());
  str.erase(std::unique(str.begin(), str.end()), str.end());
  return str;
}

} // unnamed namespace

FilenameEntry::FilenameEntry(std::string const& unequivocalFlags, char const defaultFlag) :
  unequivocalFlags(sortCharacters(unequivocalFlags)),
  defaultFlag(defaultFlag)
{
  using namespace widgets;

  fileNameEdit = new QLineEdit;
  fileNameEdit->setValidator(new FileNameValidator(fileNameEdit));

  ClickableLabel *clHelpButton = new ClickableLabel();
  clHelpButton->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(fileNameEdit->sizeHint()));
  helpButton = clHelpButton;

  infoLabel = new QLabel(tr("New filename:"));
  QHBoxLayout *editBox = new QHBoxLayout();
  editBox->addWidget(infoLabel);
  editBox->addWidget(fileNameEdit);
  editBox->addWidget(clHelpButton);

  prependInfo = new TextIconLabel(IconLabel::Icon::Information, tr("Filename will be appended with %") + defaultFlag);
  incorrectWrn = new TextIconLabel(IconLabel::Icon::Warning, tr("Filename is invalid"));

  QVBoxLayout *labels = new QVBoxLayout();
  labels->addWidget(prependInfo);
  labels->addWidget(incorrectWrn);
  labels->addStretch();

  labelAndSideWgtLayout = new QHBoxLayout();
  labelAndSideWgtLayout->addLayout(labels);

  QObject::connect(fileNameEdit, &QLineEdit::textChanged, this, &FilenameEntry::updateLabels);
  QObject::connect(clHelpButton, &ClickableLabel::clicked, this, &FilenameEntry::helpRequested);
  updateLabels();

  QVBoxLayout *vl = new QVBoxLayout();
  vl->setContentsMargins(0, 0, 0, 0);
  vl->addLayout(editBox);
  vl->addLayout(labelAndSideWgtLayout);

  setLayout(vl);
}

void FilenameEntry::setSideWidget(QWidget *widget)
{
  labelAndSideWgtLayout->addStretch();
  labelAndSideWgtLayout->addWidget(widget, 0, Qt::AlignTop);
  customWidget = widget;
}

void FilenameEntry::setEnabled(bool enable)
{
  fileNameEdit->setEnabled(enable);
  helpButton->setEnabled(enable);
  infoLabel->setEnabled(enable);

  if (!enable)
  {
    prependInfo->hide();
    incorrectWrn->hide();
  }
  else
  {
    updateLabels();
  }
}

bool FilenameEntry::isAmbiguous() const
{
  auto const& str = fileNameEdit->text();
  bool inPattern = false;

  for (auto it = str.begin(); it != str.end(); ++it)
  {
    if (*it == '%')
      inPattern = true;
    else if (inPattern)
    {
      if (std::binary_search(unequivocalFlags.begin(), unequivocalFlags.end(), it->toLatin1()))
        return false;

      inPattern = false;
    }
  }

  return true;
}

QString FilenameEntry::unequivocalSyntax() const
{
  if (!isAmbiguous())
    return fileNameEdit->text();

  return fileNameEdit->text().append('%').append(defaultFlag);
}

void FilenameEntry::updateLabels() const
{
  incorrectWrn->setVisible(!fileNameEdit->hasAcceptableInput());
  prependInfo->setVisible(isAmbiguous());
}

}} // namespace phobos::widgets
