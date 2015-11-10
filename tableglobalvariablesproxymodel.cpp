#include "tableglobalvariablesproxymodel.h"

TableGlobalVariablesProxyModel::TableGlobalVariablesProxyModel(QObject * parent)
    : QIdentityProxyModel(parent)
{
}

Qt::ItemFlags TableGlobalVariablesProxyModel::flags(QModelIndex const & index) const
{
    Qt::ItemFlags result = sourceModel()->flags(mapToSource(index));
    if (index.column() == 0) {
        result &= ~Qt::ItemIsEditable;
    }
    return result;
}
