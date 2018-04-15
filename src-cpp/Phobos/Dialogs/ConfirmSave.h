#ifndef DIALOGS_CONFIRMSAVE_H
#define DIALOGS_CONFIRMSAVE_H

#include <QDialog>

namespace phobos { namespace dialog {

class ConfirmSave : public QDialog
{
public:
  explicit ConfirmSave(QString const& filename, QWidget *parent = nullptr);

  bool dontAskAgain() const
  {
    return pDontAskAgain;
  }

private:
  bool pDontAskAgain;
};

}} // namespace phobos::dialog

#endif // DIALOGS_CONFIRMSAVE_H
