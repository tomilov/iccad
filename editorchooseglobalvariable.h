#ifndef EDITORCHOOSEGLOBALVARIABLE_H
#define EDITORCHOOSEGLOBALVARIABLE_H

#include <QStyledItemDelegate>

class EditorChooseGlobalVariable
        : public QStyledItemDelegate
{

    Q_OBJECT

public :

    explicit
    EditorChooseGlobalVariable(QObject * parent = nullptr);
    ~EditorChooseGlobalVariable() Q_DECL_OVERRIDE = default;

    QWidget * createEditor(QWidget * parent,
                          QStyleOptionViewItem const & option,
                          QModelIndex const & index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget * editor, QModelIndex const & index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget * editor, QAbstractItemModel * model,
                      QModelIndex const & index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget * editor,
                              QStyleOptionViewItem const & option,
                              QModelIndex const & index) const Q_DECL_OVERRIDE;

signals :

public slots :

};

#endif // EDITORCHOOSEGLOBALVARIABLE_H
