#include "Widgets/FilenameEntry.h"
#include "Widgets/IconLabel.h"
#include "Utils/Portable.h"
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

    return utils::portableFileName(s) ? good : QValidator::State::Invalid;
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
  vl->addLayout(editBox);
  vl->addLayout(labelAndButton);

  setLayout(vl);
}

void FilenameEntry::updateLabels() const
{
  incorrectWrn->setVisible(!fileNameEdit->hasAcceptableInput());
  prependInfo->setVisible(fileNameEdit->text().indexOf("%N") < 0);
}

}} // namespace phobos::widgets
