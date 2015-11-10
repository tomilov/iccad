#ifndef TABLEGLOBALVARIABLESPROXYMODEL_H
#define TABLEGLOBALVARIABLESPROXYMODEL_H

#include <QIdentityProxyModel>

class TableGlobalVariablesProxyModel
        : public QIdentityProxyModel
{

    Q_OBJECT

public :

    explicit
    TableGlobalVariablesProxyModel(QObject * parent = nullptr);
    ~TableGlobalVariablesProxyModel() Q_DECL_OVERRIDE = default;

    Qt::ItemFlags flags(QModelIndex const & index) const Q_DECL_OVERRIDE;

};

#endif // TABLEGLOBALVARIABLESPROXYMODEL_H
