#ifndef PROCESSWIZARD_EXECUTION_H
#define PROCESSWIZARD_EXECUTION_H

#include "ProcessWizard/ExecutionFwd.h"
#include <boost/optional.hpp>
#include <QMetaType>
#include <QString>

namespace phobos { namespace processwiz {

class Execution
{
public:
  virtual bool execute() const = 0;
  virtual QString toString() const = 0;
  virtual QString warning() const = 0;
};

}} // namespace phobos::executiuon

Q_DECLARE_METATYPE(phobos::processwiz::ConstExecutionPtrVec)

#endif // PROCESSWIZARD_EXECUTION_H
