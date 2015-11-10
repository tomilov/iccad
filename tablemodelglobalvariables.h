#ifndef TABLEMODELGLOBALVARIABLES_H
#define TABLEMODELGLOBALVARIABLES_H

#include "base_types.hpp"

#include <QAbstractTableModel>

class TableModelGlobalVariables final
        : public QAbstractTableModel
{

    Q_OBJECT

public :

    explicit
    TableModelGlobalVariables(QObject * parent = nullptr);
    ~TableModelGlobalVariables() Q_DECL_OVERRIDE = default;

    int rowCount(QModelIndex const & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(QModelIndex const & parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(QModelIndex const & index) const Q_DECL_OVERRIDE;
    bool setData(QModelIndex const & index, QVariant const & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    bool insertRows(int row, int count, QModelIndex const & parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, QModelIndex const & parent = QModelIndex()) Q_DECL_OVERRIDE;

    QVector< QPair< QString, QString > > globalVariables;

    bool
    clearData();

    bool
    populateWithData(QVector< QPair< QString, QString > > const & sourceGlobalVariables);

    bool
    populateWithValues(QVector< QVariant > const & globalVariablesValues);

};

#endif // TABLEMODELGLOBALVARIABLES_H
