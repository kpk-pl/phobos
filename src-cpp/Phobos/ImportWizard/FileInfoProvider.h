#ifndef IMPORTWIZARD_FILE_INFO_PROVIDER_H_
#define IMPORTWIZARD_FILE_INFO_PROVIDER_H_

#include "ImportWizard/Types.h"
#include <QStringList>
#include <vector>

class QWidget;

namespace phobos { namespace importwiz {

std::vector<Photo> provideFileInfo(QStringList const& photos, QWidget *parent);

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_FILE_INFO_PROVIDER_H_
