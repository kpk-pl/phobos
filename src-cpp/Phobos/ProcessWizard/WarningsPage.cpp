#include "ProcessWizard/WarningsPage.h"
#include "Widgets/IconLabel.h"
#include "Utils/Asserted.h"
#include "PhotoContainers/ItemState.h"
#include <QString>
#include <QStyle>
#include <QVBoxLayout>
#include <QAbstractButton>

namespace phobos { namespace processwiz {

WarningsPage::WarningsPage(SeriesCounts const& counts)
{
  setTitle(tr("Summary"));

  QVBoxLayout *layout = new QVBoxLayout();

  auto const maker = [this, layout](QString const& textPattern, std::size_t const num){
    if (num == 0)
      return false;

    widgets::TextIconLabel *ilabel =
        new widgets::TextIconLabel(style()->standardIcon(QStyle::SP_MessageBoxWarning), textPattern.arg(num));
    layout->addWidget(ilabel);
    return true;
  };

  layout->addWidget(new QLabel(tr("Processing %1 photos from %2 series in total")
                               .arg(counts.all.photos).arg(counts.all.series)));

  // TODO: span color tags with colors for discarded, uncheched ?
  maker(tr("Found %1 completely discarded series"), counts.seriesCompletelyDiscarded);
  maker(tr("Found %1 completely unchecked series"), counts.seriesCompletelyUnknown);
  maker(tr("Found %1 series without any selected photo"),
        counts.all.series - utils::asserted::fromMap(counts.types, pcontainer::ItemState::SELECTED).series);

  layout->addStretch();
  layout->addWidget(new QLabel(tr("Continue to next page")));

  setLayout(layout);
}

void WarningsPage::initializePage()
{
  if (wizard()->button(QWizard::NextButton))
      wizard()->button(QWizard::NextButton)->setFocus();
  else if (wizard()->button(QWizard::FinishButton))
      wizard()->button(QWizard::FinishButton)->setFocus();
}

}} // namespace phobos::processwiz
