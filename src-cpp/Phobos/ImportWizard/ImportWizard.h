#ifndef IMPORTWIZARD_IMPORTWIZARD_H
#define IMPORTWIZARD_IMPORTWIZARD_H

#include <QWizard>

namespace phobos { namespace importwiz {

class ImportWizard : public QWizard
{
    Q_OBJECT
public:
    ImportWizard(QWidget *parent = nullptr);

    QStringList const& selectedFiles() const { return _selectedFiles; }
    void loadMoreFiles();

    int exec() override;
    void accept() override;

private:
    QStringList _selectedFiles;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_IMPORTWIZARD_H
