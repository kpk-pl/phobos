#ifndef IMPORTWIZARD_DATETIMEPROVIDER_H
#define IMPORTWIZARD_DATETIMEPROVIDER_H

#include "ImportWizard/Types.h"
#include <QStringList>
#include <vector>

namespace phobos { namespace importwiz {

std::vector<Photo> provideDateTime(QStringList const& photos);

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DATETIMEPROVIDER_H
