#include "Widgets/FilenameEntry.h"
#include "Widgets/IconLabel.h"
#include "Utils/Filesystem/Portable.h"
#include <QValidator>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QPushButton>

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
  fileNameEdit = new QLineEdit;
  FileNameValidator *validator = new FileNameValidator(fileNameEdit);
  fileNameEdit->setValidator(validator);

  QHBoxLayout *editBox = new QHBoxLayout();
  editBox->addWidget(new QLabel(tr("New filename:")));
  editBox->addWidget(fileNameEdit);

  prependInfo = new widgets::TextIconLabel(style()->standardIcon(QStyle::SP_MessageBoxInformation),
                                           tr("Filename will be appended with %") + defaultFlag);

  incorrectWrn = new widgets::TextIconLabel(style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                            tr("Filename is invalid"));
  QVBoxLayout *labels = new QVBoxLayout();
  labels->addWidget(prependInfo);
  labels->addWidget(incorrectWrn);

  QPushButton *helpButton = new QPushButton(tr("Help"));
  helpButton->setEnabled(false);
  // TODO: add syntax help to button

  QHBoxLayout *labelAndButton = new QHBoxLayout();
  labelAndButton->addLayout(labels);
  labelAndButton->addStretch();
  labelAndButton->addWidget(helpButton, 0, Qt::AlignTop);

  QObject::connect(fileNameEdit, &QLineEdit::textChanged, this, &FilenameEntry::updateLabels);
  updateLabels();

  QVBoxLayout *vl = new QVBoxLayout();
  vl->setContentsMargins(0, 0, 0, 0);
  vl->addLayout(editBox);
  vl->addLayout(labelAndButton);

  setLayout(vl);
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
