#ifndef TREEMODELTRANFSORMATIONS_H
#define TREEMODELTRANFSORMATIONS_H

#include <QAbstractItemModel>

#include <memory>

class TreeModelTranfsormations
        : public QAbstractItemModel
{

    Q_OBJECT

public :

    struct Node;

    using NodeList = QVector< std::shared_ptr< Node > >;

    struct Node
    {

        Node * parent;
        int wrt;
        bool postSimplify;
        bool used;

        NodeList children;

    };

    QVector< QPair< QString, QString > > wrts;
    NodeList nodes;

    explicit
    TreeModelTranfsormations(QObject * parent = nullptr);
    ~TreeModelTranfsormations() Q_DECL_OVERRIDE = default;

    QModelIndex index(int row, int column,
                      QModelIndex const & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(QModelIndex const & child) const Q_DECL_OVERRIDE;

    int rowCount(QModelIndex const & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(QModelIndex const & parent = QModelIndex()) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(QModelIndex const & index) const Q_DECL_OVERRIDE;
    QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(QModelIndex const & index, QVariant const & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;

    bool insertRows(int position, int count, QModelIndex const & parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int count, QModelIndex const & parent = QModelIndex()) Q_DECL_OVERRIDE;

    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData * mimeData(QModelIndexList const & indexes) const Q_DECL_OVERRIDE;
    bool dropMimeData(QMimeData const * data, Qt::DropAction action,
                      int row, int column, QModelIndex const & parent) Q_DECL_OVERRIDE;
    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;

signals :

public slots :

    void setWrts(QVector< QPair< QString, QString > > const & globalVariables);

private :

    QString const mimeType;

    int
    findRow(Node const * nodeInfo) const;

};

#endif // TREEMODELTRANFSORMATIONS_H
