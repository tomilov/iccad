#include "editorchooseglobalvariable.h"
#include "treemodeltranfsormations.h"

#include <QComboBox>

EditorChooseGlobalVariable::EditorChooseGlobalVariable(QObject * parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *
EditorChooseGlobalVariable::createEditor(QWidget * parent,
                                         QStyleOptionViewItem const & option,
                                         QModelIndex const & index) const
{
    TreeModelTranfsormations const * model = static_cast< TreeModelTranfsormations const * >(index.model());
    QComboBox * editor = new QComboBox(parent);
    editor->setGeometry(option.rect);
    for (QPair< QString, QString > const & wrt : model->wrts) {
        editor->addItem(wrt.first);
    }
    return editor;
}

void
EditorChooseGlobalVariable::setEditorData(QWidget * editor, QModelIndex const & index) const
{
    QAbstractItemModel const * model = index.model();
    bool ok = false;
    int wrt = model->data(index, Qt::EditRole).toInt(&ok);
    Q_ASSERT(ok);
    QComboBox * comboBox = static_cast< QComboBox * >(editor);
    comboBox->setCurrentIndex(wrt);
}

void
EditorChooseGlobalVariable::setModelData(QWidget * editor, QAbstractItemModel * model,
                                         QModelIndex const & index) const
{
    QComboBox * comboBox = static_cast< QComboBox * >(editor);
    int wrt = comboBox->currentIndex();
    model->setData(index, QVariant(wrt));
}

void
EditorChooseGlobalVariable::updateEditorGeometry(QWidget * editor,
                                                 QStyleOptionViewItem const & option,
                                                 QModelIndex const & index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
