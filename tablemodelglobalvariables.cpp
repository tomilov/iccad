#include "tablemodelglobalvariables.h"
#include "settings.h"

#include <QApplication>
#include <QPalette>
#include <QDebug>

TableModelGlobalVariables::TableModelGlobalVariables(QObject * parent)
    : QAbstractTableModel(parent)
{ ; }

int TableModelGlobalVariables::rowCount(QModelIndex const & parent) const
{
    Q_UNUSED(parent);
    return globalVariables.count();
}

int TableModelGlobalVariables::columnCount(QModelIndex const & parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant TableModelGlobalVariables::data(QModelIndex const & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int row = index.row();
    if (!(row < globalVariables.size())) {
        return QVariant();
    }
    QPair< QString, QString > const & globalVariable = globalVariables[row];
    int col = index.column();
    switch (role) {
    case Qt::DisplayRole : {
        switch (col) {
        case 0 : {
            return globalVariable.first;
        }
        case 1 : {
            return globalVariable.second;
        }
        default : {
            break;
        }
        }
        break;
    }
    case Qt::ToolTipRole :
    case Qt::StatusTipRole : {
        switch (col) {
        case 0 : {
            return "Global variable name with value " + globalVariable.second;
        }
        case 1 : {
            return "Value of global variable " + globalVariable.first;
        }
        default : {
            break;
        }
        }
        break;
    }
    case Qt::BackgroundRole : {
        if (col == 1) {
            return qApp->palette().alternateBase();
        } else {
            return qApp->palette().base();
        }
    }
    default : {
        break;
    }
    }
    return QVariant();
}

QVariant TableModelGlobalVariables::headerData(int section,
                                               Qt::Orientation orientation,
                                               int role) const
{
    switch (role) {
    case Qt::DisplayRole : {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0 : {
                return tr("Name");
            }
            case 1 : {
                return tr("Value");
            }
            default : {
                break;
            }
            }
        }
        break;
    }
    }
    return QVariant();
}

Qt::ItemFlags TableModelGlobalVariables::flags(QModelIndex const & index) const
{
    Qt::ItemFlags result = QAbstractItemModel::flags(index);
    result |= Qt::ItemIsEditable;
    return result;
}

bool TableModelGlobalVariables::setData(QModelIndex const & index,
                                        QVariant const & value,
                                        int role)
{
    if (role != Qt::EditRole) {
        return false;
    }
    if (!index.isValid()) {
        return false;
    }
    int row = index.row();
    if (!(row < globalVariables.size())) {
        return false;
    }
    QPair< QString, QString > & globalVariable_ = globalVariables[row];
    switch (index.column()) {
    case 0 : {
        if (!value.canConvert(QVariant::String)) {
            qWarning() << "Variant contains non string value";
            return false;
        }
        QString name = value.toString();
        if (!Settings::getPersistentSettings().symbolExpression.exactMatch(name)) {
            qWarning() << name + " is not correct symbol name";
            return false;
        }
        if (Settings::getPersistentSettings().dummySymbolExpression.exactMatch(name)) {
            qWarning() << "The underscore symbol used as dummy variable name";
            return false;
        }
        if (Settings::getPersistentSettings().reservedSymbols.contains(name)) {
            qWarning() << "Symbol " + name + " is reserved";
            return false;
        }
        for (QPair< QString, QString > const & globalVariable : globalVariables) {
            if (globalVariable.first == name) {
                qWarning() << "A duplicates of symbols are not allowed";
                return false;
            }
        }
        globalVariable_.first = name;
        break;
    }
    case 1 : {
        if (!value.canConvert(QVariant::String)) {
            qWarning() << "Variant contains non string value";
            return false;
        }
        QString variableValue = value.toString();
        if (!variableValue.isEmpty()) {
            if (!Settings::getPersistentSettings().floatingPointExpression.exactMatch(variableValue)) {
                qWarning() << "Variant contains string which is nonconvertible to floating-point number";
                return false;
            }
        }
        globalVariable_.second = variableValue;
        break;
    }
    default : {
        return false;
    }
    }
    emit dataChanged(index, index);
    return true;
}

bool TableModelGlobalVariables::insertRows(int row, int count, QModelIndex const & parent)
{
    if (count < 1) {
        return false;
    }
    int size = globalVariables.count();
    if (row < 0) {
        row = size;
    }
    if (size < row) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    globalVariables.insert(row, count, QPair< QString, QString >());
    endInsertRows();
    return true;
}

bool TableModelGlobalVariables::removeRows(int row, int count, QModelIndex const & parent)
{
    if (count < 1) {
        return false;
    }
    if (row < 0) {
        return false;
    }
    int size = globalVariables.count();
    if (size < 0) {
        return false;
    }
    if (size < row + count) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    globalVariables.remove(row, count);
    endRemoveRows();
    return true;
}

bool
TableModelGlobalVariables::clearData()
{
    return removeRows(0, globalVariables.size());
}

bool
TableModelGlobalVariables::populateWithData(QVector< QPair< QString, QString > > const & sourceGlobalVariables)
{
    clearData();
    int count = sourceGlobalVariables.size();
    if (!insertRows(0, count)) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        QPair< QString, QString > const & sourceGlobalVariable = sourceGlobalVariables[i];
        if (!setData(index(i, 0), sourceGlobalVariable.first)) {
            return false;
        }
        if (!setData(index(i, 1), sourceGlobalVariable.second)) {
            return false;
        }
    }
    return true;
}

bool
TableModelGlobalVariables::populateWithValues(QVector< QVariant > const & globalVariablesValues)
{
    int count = globalVariables.size();
    for (int i = 0; i < count; ++i) {
        if (!setData(index(i, 1), globalVariablesValues[i])) {
            return false;
        }
    }
    return true;
}
