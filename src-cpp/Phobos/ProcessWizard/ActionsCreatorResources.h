#ifndef PROCESSWIZARD_ACTIONSCREATORRESOURCES_H
#define PROCESSWIZARD_ACTIONSCREATORRESOURCES_H

#include <QObject>

class QDialog;
class QWidget;

namespace phobos { namespace processwiz {

class ActionsCreatorResources : public QObject
{
  Q_OBJECT

public:
  explicit ActionsCreatorResources(QWidget *parent);

public slots:
  void showRenameSyntaxHelp();

private:
  void createRenameSyntaxDialog();

  QWidget *parent;
  QDialog *renameSyntaxDialog = nullptr;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_ACTIONSCREATORRESOURCES_H
