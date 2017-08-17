#ifndef PROCESSWIZARD_TYPEACTIONTAB_H
#define PROCESSWIZARD_TYPEACTIONTAB_H

#include "PhotoContainers/ItemState.h"
#include "ProcessWizard/Action.h"
#include <QWidget>

class QListWidget;

namespace phobos { namespace processwiz {

class TypeActionTab : public QWidget
{
  Q_OBJECT

public:
  TypeActionTab(pcontainer::ItemState const matchState);

  std::size_t activeActions() const;

signals:
  void actionsChanged();

public slots:
  void clearActions();

private slots:
  void acceptNewAction(ConstActionPtr action);
  void selectivelyDisableActions();

private:
  using ActionWithOrder = std::pair<ConstActionPtr, std::size_t>;
  struct ActionWithOrderComp;

  std::vector<ActionWithOrder> actions;
  QListWidget *listWidget;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_TYPEACTIONTAB_H
