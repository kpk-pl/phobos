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
    QValidator::State good = QValidator::State::Acceptable;
    std::string s = input.toStdString();

    if (s.empty())
      return QValidator::State::Intermediate;
    else if (!s.empty() && *(s.end()-1) == '.')
    {
      s.erase(s.end()-1);
      good = QValidator::State::Intermediate;
    }

    // Replace '%.' sequences with acceptable input characters
    for (std::size_t pos = 0; pos < s.length(); ++pos)
      if (s[pos] == '%')
        s.replace(pos, 2, 2, '_');

    return utils::fs::portableFileName(s) ? good : QValidator::State::Invalid;
  }
};
} // unnamed namespace

FilenameEntry::FilenameEntry()
{
  fileNameEdit = new QLineEdit;
  FileNameValidator *validator = new FileNameValidator(fileNameEdit);
  fileNameEdit->setValidator(validator);

  QHBoxLayout *editBox = new QHBoxLayout();
  editBox->addWidget(new QLabel(tr("New filename:")));
  editBox->addWidget(fileNameEdit);

  prependInfo = new widgets::TextIconLabel(style()->standardIcon(QStyle::SP_MessageBoxInformation),
                                           tr("Filename will be prepended with %N"));

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

namespace {
  QString unequivocalPatterns[] = { "%N", "%n", "%F" };

  bool isAmbiguous(QString const& str)
  {
    bool ambiguous = true;
    for (auto const& pattern : unequivocalPatterns)
      ambiguous = ambiguous && (str.indexOf(pattern) < 0);

    return ambiguous;
  }
} // unnamed namespace

QString FilenameEntry::unequivocalSyntax() const
{
  QString str = fileNameEdit->text();
  if (isAmbiguous(str))
    str.prepend("%N");

  return str;
}

void FilenameEntry::updateLabels() const
{
  incorrectWrn->setVisible(!fileNameEdit->hasAcceptableInput());
  prependInfo->setVisible(isAmbiguous(fileNameEdit->text()));
}

}} // namespace phobos::widgets
