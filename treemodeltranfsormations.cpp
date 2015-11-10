#include "treemodeltranfsormations.h"

#include <QDebug>
#include <QColor>
#include <QMimeData>

#include <algorithm>

TreeModelTranfsormations::TreeModelTranfsormations(QObject * parent)
    : QAbstractItemModel(parent)
    , mimeType("application/vnd.items.tree")
{
}

void
TreeModelTranfsormations::setWrts(QVector< QPair< QString, QString > > const & globalVariables)
{
    wrts = globalVariables;
    emit dataChanged(QModelIndex(), QModelIndex());
}

QModelIndex
TreeModelTranfsormations::index(int row, int column,
                                QModelIndex const & parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    if (parent.isValid()) {
        Node * parentNode = static_cast< Node * >(parent.internalPointer());
        Q_ASSERT(parentNode != nullptr);
        return createIndex(row, column, parentNode->children[row].get());
    } else {
        return createIndex(row, column, const_cast< Node * >(nodes.at(row).get()));
    }
}

QModelIndex
TreeModelTranfsormations::parent(QModelIndex const & child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }
    Node * childNode = static_cast< Node * >(child.internalPointer());
    Q_ASSERT(childNode != nullptr);
    Node * parentNode = childNode->parent;
    if (parentNode == nullptr) {
        return QModelIndex();
    }
    return createIndex(findRow(parentNode), 0, parentNode);
}

int
TreeModelTranfsormations::findRow(Node const * node) const
{
    NodeList const & parentNodeChildren = (node->parent != nullptr) ? node->parent->children : nodes;
    int i = 0;
    for (std::shared_ptr< Node > const & childNode : parentNodeChildren) {
        if (childNode.get() == node) {
            return i;
        }
        ++i;
    }
    Q_UNREACHABLE();
}

int
TreeModelTranfsormations::rowCount(QModelIndex const & parent) const
{
    if (parent.isValid()) {
        Node const * parentNode = static_cast< Node const * >(parent.internalPointer());
        Q_ASSERT(parentNode != nullptr);
        return parentNode->children.size();
    } else {
        return nodes.size();
    }
}

int
TreeModelTranfsormations::columnCount(QModelIndex const & parent) const
{
    Q_UNUSED(parent);
    return 3;
}

Qt::ItemFlags
TreeModelTranfsormations::flags(QModelIndex const & index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        switch (index.column()) {
        case 0 : {
            flags |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
            break;
        }
        case 1 : {
            flags |= Qt::ItemIsUserCheckable;
            break;
        }
        case 2 : {
            flags |= Qt::ItemIsUserCheckable;
            break;
        }
        default : {
            break;
        }
        }
    } else {
        flags |= Qt::ItemIsDropEnabled;
    }
    return flags;
}

QVariant
TreeModelTranfsormations::data(QModelIndex const & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Node const * node = static_cast< Node const * >(index.internalPointer());
    Q_ASSERT(node != nullptr);
    switch (index.column()) {
    case 0 : {
        int wrt = node->wrt;
        if (!(wrt < wrts.size())) {
            wrt = -1;
        }
        switch (role) {
        case Qt::DisplayRole : {
            if (!(wrt < 0) && (wrt < wrts.size())) {
                return wrts.at(wrt).first;
            } else {
                return QString("<!>");
            }
            //break;
        }
        case Qt::EditRole : {
            return wrt;
        }
        case Qt::TextAlignmentRole : {
            return Qt::AlignLeft;
        }
        case Qt::TextColorRole : {
            if (wrt < 0) {
                return QColor(Qt::red);
            } else {
                return QColor(Qt::blue);
            }
            //break;
        }
        default : {
            break;
        }
        }
        break;
    }
    case 1 : {
        switch (role) {
        case Qt::CheckStateRole : {
            if (node->postSimplify) {
                return Qt::Checked;
            } else {
                return Qt::Unchecked;
            }
        }
        case Qt::DisplayRole : {
            if (node->postSimplify) {
                return "simplify";
            } else {
                return "pass";
            }
        }
        case Qt::TextAlignmentRole : {
            return Qt::AlignLeft;
        }
        default : {
            break;
        }
        }
        break;
    }
    case 2 : {
        switch (role) {
        case Qt::CheckStateRole : {
            if (node->used) {
                return Qt::Checked;
            } else {
                return Qt::Unchecked;
            }
        }
        case Qt::DisplayRole : {
            if (node->used) {
                return "used";
            } else {
                return "unused";
            }
        }
        case Qt::TextAlignmentRole : {
            return Qt::AlignLeft;
        }
        default : {
            break;
        }
        }
        break;
    }
    default : {
        break;
    }
    }
    return QVariant();
}

bool
TreeModelTranfsormations::setData(QModelIndex const & index, QVariant const & value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    Node * node = static_cast< Node * >(index.internalPointer());
    Q_ASSERT(node != nullptr);
    switch (index.column()) {
    case 0 : {
        if (role != Qt::EditRole) {
            return false;
        }
        if (!value.canConvert(QVariant::Int)) {
            return false;
        }
        bool ok = false;
        node->wrt = value.toInt(&ok);
        Q_ASSERT(ok);
        break;
    }
    case 1 : {
        if (role != Qt::CheckStateRole) {
            return false;
        }
        if (!value.canConvert(QVariant::Int)) {
            return false;
        }
        bool ok = false;
        Qt::CheckState state = static_cast< Qt::CheckState >(value.toInt(&ok));
        Q_ASSERT(ok);
        switch (state) {
        case Qt::Checked : {
            node->postSimplify = true;
            break;
        }
        case Qt::Unchecked : {
            node->postSimplify = false;
            break;
        }
        case Qt::PartiallyChecked : {
            return false;
        }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"
        default : {
#pragma clang diagnostic pop
            return false;
        }
        }
        break;
    }
    case 2 : {
        if (role != Qt::CheckStateRole) {
            return false;
        }
        if (!value.canConvert(QVariant::Int)) {
            return false;
        }
        bool ok = false;
        Qt::CheckState state = static_cast< Qt::CheckState >(value.toInt(&ok));
        Q_ASSERT(ok);
        switch (state) {
        case Qt::Checked : {
            node->used = true;
            QModelIndex parent = index.parent();
            if (parent.isValid()) {
                if (!setData(TreeModelTranfsormations::index(parent.row(), 2, parent.parent()), Qt::Checked, Qt::CheckStateRole)) {
                    qWarning() << "cannot set parent Use fild";
                }
            }
            break;
        }
        case Qt::Unchecked : {
            node->used = false;
            break;
        }
        case Qt::PartiallyChecked : {
            return false;
        }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"
        default : {
#pragma clang diagnostic pop
            return false;
        }
        }
        break;
    }
    default : {
        return false;
    }
    }
    emit dataChanged(index, index);
    return true;
}

QVariant
TreeModelTranfsormations::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole : {
        switch (section) {
        case 0 : {
            return "WRT";
        }
        case 1 : {
            return "Postsimplify";
        }
        case 2 : {
            return "Use";
        }
        default : {
            break;
        }
        }
        break;
    }
    case Qt::TextAlignmentRole : {
        return Qt::AlignCenter;
    }
    default : {
        break;
    }
    }
    return QVariant();
}

bool
TreeModelTranfsormations::insertRows(int position, int count, QModelIndex const & parent)
{
    if (count < 1) {
        return false;
    }
    int size = rowCount(parent);
    if (size < position) {
        return false;
    }
    if (position < 0) {
        position = size;
    }
    beginInsertRows(parent, position, position + count - 1);
    if (parent.isValid()) {
        Node * parentNode = static_cast< Node * >(parent.internalPointer());
        Q_ASSERT(parentNode != nullptr);
        parentNode->children.insert(position, count, std::shared_ptr< Node >(new Node{parentNode, -1, true, true, {}}));
    } else {
        nodes.insert(position, count, std::shared_ptr< Node >(new Node{nullptr, -1, true, true, {}}));
    }
    endInsertRows();
    return true;
}

bool
TreeModelTranfsormations::removeRows(int position, int count, QModelIndex const & parent)
{
    if (position < 0) {
        return false;
    }
    if (count < 1) {
        return false;
    }
    if (rowCount(parent) < position + count) {
        return false;
    }
    beginRemoveRows(parent, position, position + count - 1);
    if (parent.isValid()) {
        Node * parentNode = static_cast< Node * >(parent.internalPointer());
        Q_ASSERT(parentNode != nullptr);
        parentNode->children.remove(position, count);
    } else {
        nodes.remove(position, count);
    }
    endRemoveRows();
    return true;
}

QStringList
TreeModelTranfsormations::mimeTypes() const
{
    return QStringList(mimeType);
}

QMimeData *
TreeModelTranfsormations::mimeData(QModelIndexList const & indexes) const
{
    QMimeData * mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    for (QModelIndex const & index : indexes) {
        stream << index.row() << index.column() << index.internalId();
    }
    mimeData->setData(mimeType, encodedData);
    return mimeData;
}

bool
TreeModelTranfsormations::dropMimeData(QMimeData const * data, Qt::DropAction action,
                                       int row, int column, QModelIndex const & parent)
{
    Q_ASSERT(data != nullptr);
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if (action != Qt::MoveAction) {
        return true;
    }
    if (!data->hasFormat(mimeType)) {
        return false;
    }
    if (column != 0) {
        //return false;
    }
    int beginRow = -1;
    if (!(row < 0)) {
        beginRow = row;
    } else if (parent.isValid()) {
        beginRow = parent.row();
    } else {
        beginRow = rowCount();
    }
    // decode
    QModelIndexList indexes;
    {
        QByteArray encoded = data->data(mimeType);
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        while (!stream.atEnd()) {
            int row_ = -1;
            int column_ = -1;
            static_assert(sizeof(quintptr) == sizeof(nullptr));
            quintptr internalId = reinterpret_cast< quintptr >(nullptr);
            stream >> row_ >> column_ >> internalId;
            indexes.append(createIndex(row_, column_, internalId));
        }
    }
    //qDebug() << indexes;
    if (indexes.empty()) {
        return true;
    }
    //insertRows(beginRow, indexes.size(), parent);
    return false;
}

Qt::DropActions
TreeModelTranfsormations::supportedDropActions() const
{
    return Qt::MoveAction;
}
