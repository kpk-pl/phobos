#include "ProcessWizard/WarningsPage.h"
#include "Widgets/IconLabel.h"
#include "PhotoContainers/ItemState.h"
#include <QString>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <easylogging++.h>

namespace phobos { namespace processwiz {

WarningsPage::WarningsPage(SeriesCounts const& counts)
{
  LOG(DEBUG) << "Creating page";

  setTitle(tr("Summary"));

  QVBoxLayout *layout = new QVBoxLayout();

  auto const maker = [this, layout](QString const& textPattern, std::size_t const num){
    if (num == 0)
      return false;

    widgets::TextIconLabel *ilabel =
        new widgets::TextIconLabel(widgets::IconLabel::Icon::Warning, textPattern.arg(num));
    layout->addWidget(ilabel);
    return true;
  };

  layout->addWidget(new QLabel(tr("Processing %1 photos from %2 series in total")
                               .arg(counts.all.photos).arg(counts.all.series)));

  // TODO: display how many selected, unknown photos are there
  // TODO: span color tags with colors for discarded, uncheched ?
  bool warningsPresent = false;
  warningsPresent |= maker(tr("Found %1 series without any selected photo"), counts.all.series - counts.selected.series);

  layout->addStretch();
  layout->addWidget(new QLabel(tr("Continue to next page")));

  if (warningsPresent)
    setButtonText(QWizard::WizardButton::NextButton, tr("Ignore warnings"));

  setLayout(layout);
}

void WarningsPage::initializePage()
{
  LOG(INFO) << "Initializing warnings page";

  if (wizard()->button(QWizard::NextButton))
      wizard()->button(QWizard::NextButton)->setFocus();
}

bool WarningsPage::validatePage()
{
  LOG(INFO) << "Ignored warnings and moved forward";
  return true;
}

}} // namespace phobos::processwiz
