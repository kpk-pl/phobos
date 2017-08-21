#include "ProcessWizard/Action.h"
#include "ProcessWizard/ExecutionImpl.h"
#include "ProcessWizard/SeriesCounts.h"
#include "PhotoContainers/Set.h"
#include "PhotoContainers/Series.h"
#include "PhotoContainers/Item.h"
#include "Utils/LexicalCast.h"
#include <easylogging++.h>
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <cassert>
#include <cmath>

namespace phobos { namespace processwiz {

Action::Action(pcontainer::ItemState const matchedState) :
  matchedState(matchedState)
{
}

pcontainer::ItemState Action::matching() const
{
  return matchedState;
}

bool Action::operator<(Action const& other) const
{
  return priority() < other.priority();
}

DeleteAction::DeleteAction(pcontainer::ItemState const matchedState, Method const method) :
  Action(matchedState), method(method)
{
}

QString DeleteAction::toString() const
{
  switch(method) {
  case Method::Permanent:
    return QObject::tr("Delete %1 photos permanently")
                    .arg(QString::fromStdString(utils::lexicalCast(matchedState)));
  case Method::Trash:
    return QObject::tr("Move %1 photos to trash")
                    .arg(QString::fromStdString(utils::lexicalCast(matchedState)));
  }

  assert(false);
  return QString();
}

ConstExecutionPtrVecConstPtr
  DeleteAction::makeExecutions(pcontainer::Set const& photoSet,
                               SeriesCounts const&) const
{
  auto result = std::make_shared<ConstExecutionPtrVec>();

  for (pcontainer::SeriesPtr const& series : photoSet)
    for (pcontainer::ItemPtr const& photo : *series)
      if (photo->state() == matchedState)
        result->push_back(std::make_shared<DeleteExecution>(photo->fileName(), method));

  return result;
}

namespace {
  struct RenameBits
  {
    std::size_t allAvailablePhotos = 0;
    std::size_t processedPhotos = 0;
    std::size_t affectedPhotos = 0;
  };

  int numDigits(std::size_t const val)
  {
    if (val < 10) return 1;
    if (val < 100) return 2;
    if (val < 1000) return 3;
    if (val < 10000) return 4;
    return static_cast<int>(std::log10(val))+1;
  }

  QString replacementPattern(char const signature, RenameBits const& bits, QString const& origFilename)
  {
    switch(signature)
    {
    case 'N':
      return QString("%1").arg(bits.processedPhotos, numDigits(bits.allAvailablePhotos), 10, QChar('0'));
    case 'n':
      return QString("%1").arg(bits.affectedPhotos, numDigits(bits.allAvailablePhotos), 10, QChar('0'));
    case 'F':
      return origFilename;
    default:
      return QString{};
    }
  }

  QString renameFile(QString const& file, QString pattern, RenameBits const& bits)
  {
    QFileInfo const fInfo(file);
    QString const originalFilename = fInfo.baseName();

    while(true)
    {
      int const replSeq = pattern.indexOf('%');
      if (replSeq < 0 || replSeq > pattern.length()-2)
        break;

      pattern.replace(replSeq, 2, replacementPattern(pattern.at(replSeq+1).toLatin1(), bits, originalFilename));
    }

    if (pattern.endsWith('%'))
      pattern = pattern.left(pattern.length()-1);

    return pattern+'.'+fInfo.completeSuffix();
  }
} // unnamed namespace

RenameAction::RenameAction(const pcontainer::ItemState matchedState, QString const& pattern) :
  Action(matchedState), pattern(pattern)
{
}

QString RenameAction::toString() const
{
  return QObject::tr("Rename each %1 photo to \"%2\"")
      .arg(QString::fromStdString(utils::lexicalCast(matchedState)), pattern);
}

ConstExecutionPtrVecConstPtr
  RenameAction::makeExecutions(pcontainer::Set const& photoSet,
                               SeriesCounts const& seriesCounts) const
{
  auto result = std::make_shared<ConstExecutionPtrVec>();

  RenameBits bits = {};
  bits.allAvailablePhotos = seriesCounts.all.photos;

  for (pcontainer::SeriesPtr const& series : photoSet)
    for (pcontainer::ItemPtr const& photo : *series)
    {
      if (photo->state() == matchedState)
      {
        QString const newFileName = renameFile(photo->fileName(), pattern, bits);
        result->push_back(std::make_shared<RenameExecution>(photo->fileName(), newFileName));
        ++bits.affectedPhotos;
      }
      ++bits.processedPhotos;
    }

  return result;
}

}} // namespace phobos::processwiz
