#ifndef PROCESSWIZARD_OPERATIONICON_H
#define PROCESSWIZARD_OPERATIONICON_H

#include "ProcessWizard/Operation.h"
#include <QPixmap>

namespace phobos { namespace processwiz {

QPixmap operationIcon(OperationType const& operation, QSize const& size = QSize());

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_OPERATIONICON_H
