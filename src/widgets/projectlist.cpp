#include "widgets/projectlist.h"
#include "widgets/projectlistitemdelegate.h"
#include "base/application.h"

ProjectList::ProjectList(QWidget *parent) : QComboBox(parent)
{
#if defined(Q_OS_WIN)
    setMinimumHeight(app->scale(40));
    setIconSize(app->scale(32, 32));
#elif defined(Q_OS_OSX)
    setIconSize(app->scale(16, 16));
#else
    setMinimumHeight(app->scale(46));
    setIconSize(app->scale(32, 32));
#endif

    setItemDelegate(new ProjectListItemDelegate(this));
}

void ProjectList::setModel(QAbstractItemModel *newModel)
{
    auto previousModel = model();
    if (previousModel) {
        disconnect(previousModel, &ProjectItemsModel::rowsInserted, this, &ProjectList::onRowsInserted);
    }
    QComboBox::setModel(newModel);
    if (newModel) {
        connect(newModel, &ProjectItemsModel::rowsInserted, this, &ProjectList::onRowsInserted);
    }
}

bool ProjectList::setCurrentProject(Project *project)
{
    const int rowCount = model()->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        if (project == model()->index(i, 0).internalPointer()) {
            setCurrentIndex(i);
            return true;
        }
    }
    return false;
}

void ProjectList::onRowsInserted(const QModelIndex &, int, int last)
{
    setCurrentIndex(last);
}
