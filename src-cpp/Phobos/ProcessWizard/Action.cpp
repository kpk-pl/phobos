#include "ProcessWizard/Action.h"
#include "ProcessWizard/Execution/ExecutionImpl.h"
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
#include <functional>

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
{}

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

ConstExecutionPtrVec
  DeleteAction::makeExecutions(pcontainer::Set const& photoSet,
                               SeriesCounts const&) const
{
  ConstExecutionPtrVec result;

  for (pcontainer::SeriesPtr const& series : photoSet)
    for (pcontainer::ItemPtr const& photo : *series)
      if (photo->state() == matchedState)
        result.emplace_back(std::make_shared<DeleteExecution>(photo->id(), method));

  return result;
}

RenameAction::RenameAction(const pcontainer::ItemState matchedState, QString const& pattern) :
  Action(matchedState), pattern(pattern)
{}

QString RenameAction::toString() const
{
  return QObject::tr("Rename each %1 photo to \"%2\"")
      .arg(QString::fromStdString(utils::lexicalCast(matchedState)), pattern);
}

namespace {
int numDigits(std::size_t const val)
{
  if (val < 10) return 1;
  if (val < 100) return 2;
  if (val < 1000) return 3;
  if (val < 10000) return 4;
  return static_cast<int>(std::log10(val))+1;
}

class RenameProcessor
{
public:
  RenameProcessor(pcontainer::ItemState const matchedState, SeriesCounts const& seriesCounts) :
    matchedState(matchedState), allAvailablePhotos(seriesCounts.allSeries.photos + seriesCounts.allFree.photos)
  {}

  void setRenamePattern(QString const& pattern)
  {
    renamePattern = pattern;
  }

  ConstExecutionPtrVec operator()(pcontainer::Set const& photoSet,
                                  std::function<ConstExecutionPtr(pcontainer::ItemId const&, QString const&)> const& executionMaker)
  {
    ConstExecutionPtrVec result;

    for (pcontainer::SeriesPtr const& series : photoSet)
      for (pcontainer::ItemPtr const& photo : *series)
      {
        if (photo->state() == matchedState)
        {
          result.emplace_back(executionMaker(photo->id(), processPattern(photo->fileName())));
          ++affectedPhotos;
        }
        ++processedPhotos;
      }

    return result;
  }

private:
  QString processPattern(QString const& fileName)
  {
    QFileInfo const fInfo(fileName);
    QString const originalFilename = fInfo.fileName();

    if (renamePattern.isEmpty())
      return originalFilename;

    QString const originalBasename = fInfo.baseName();
    QString pattern = renamePattern;

    while(true)
    {
      int const replSeq = pattern.indexOf('%');
      if (replSeq < 0 || replSeq > pattern.length()-2)
        break;

      pattern.replace(replSeq, 2, expandPattern(pattern.at(replSeq+1).toLatin1(), originalBasename));
    }

    if (pattern.endsWith('%'))
      pattern = pattern.left(pattern.length()-1);

    return pattern+'.'+fInfo.completeSuffix();
  }

  QString expandPattern(char const signature, QString const& originalBasename)
  {
    switch(signature)
    {
    case 'N':
      return QString("%1").arg(processedPhotos, numDigits(allAvailablePhotos), 10, QChar('0'));
    case 'n':
      return QString("%1").arg(affectedPhotos, numDigits(allAvailablePhotos), 10, QChar('0'));
    case 'F':
      return originalBasename;
    default:
      return QString{};
    }
  }

  pcontainer::ItemState const matchedState;

  std::size_t const allAvailablePhotos;
  std::size_t processedPhotos = 0;
  std::size_t affectedPhotos = 0;

  QString renamePattern;
};
} // unnamed namespace

ConstExecutionPtrVec
  RenameAction::makeExecutions(pcontainer::Set const& photoSet,
                               SeriesCounts const& seriesCounts) const
{
  RenameProcessor processor(matchedState, seriesCounts);
  processor.setRenamePattern(pattern);

  return processor(photoSet, [](auto const& id, QString const& fn){
    return std::make_shared<RenameExecution>(id, QFileInfo(id.fileName).dir().filePath(fn));
  });
}

MoveAction::MoveAction(pcontainer::ItemState const matchedState,
                       QDir const& destination,
                       QString const& optRenamePattern) :
  Action(matchedState), destination(destination), optPattern(optRenamePattern)
{}

QString MoveAction::toString() const
{
  QString result = QObject::tr("Move each %1 photo to \"%2\"")
      .arg(QString::fromStdString(utils::lexicalCast(matchedState)), destination.path());

  if (!optPattern.isEmpty())
    result.append(QObject::tr(" and rename to \"%1\"").arg(optPattern));

  return result;
}

ConstExecutionPtrVec
  MoveAction::makeExecutions(pcontainer::Set const& photoSet,
                             SeriesCounts const& counts) const
{
  RenameProcessor processor(matchedState, counts);

  if (!optPattern.isEmpty())
    processor.setRenamePattern(optPattern);

  return processor(photoSet, [dest=destination](auto const& id, QString const& fn){
    return std::make_shared<RenameExecution>(id, dest.filePath(fn));
  });
}

CopyAction::CopyAction(pcontainer::ItemState const matchedState,
                       QDir const& destination,
                       QString const& optRenamePattern) :
  Action(matchedState), destination(destination), optPattern(optRenamePattern)
{}

QString CopyAction::toString() const
{
  QString result = QObject::tr("Copy each %1 photo to \"%2\"")
      .arg(QString::fromStdString(utils::lexicalCast(matchedState)), destination.path());

  if (!optPattern.isEmpty())
    result.append(QObject::tr(" and rename copy to \"%1\"").arg(optPattern));

  return result;
}

ConstExecutionPtrVec
  CopyAction::makeExecutions(pcontainer::Set const& photoSet,
                             SeriesCounts const& counts) const
{
  RenameProcessor processor(matchedState, counts);

  if (!optPattern.isEmpty())
    processor.setRenamePattern(optPattern);

  return processor(photoSet, [dest=destination](auto const& id, QString const& fn){
    return std::make_shared<CopyExecution>(id, dest.filePath(fn));
  });
}

}} // namespace phobos::processwiz
