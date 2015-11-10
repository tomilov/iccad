#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base_types.hpp"
#include "tablemodelglobalvariables.h"
#include "tableglobalvariablesproxymodel.h"
#include "treemodeltranfsormations.h"
#include "editorchooseglobalvariable.h"

#include <insituc/parser/parser.hpp>
#include <insituc/meta/assembler.hpp>
#include <insituc/meta/compiler.hpp>
#include <insituc/transform/evaluator/evaluator.hpp>
#include <insituc/transform/derivator/derivator.hpp>
#include <insituc/runtime/jit_compiler/instance.hpp>
#include <insituc/runtime/jit_compiler/translator.hpp>
#include <insituc/runtime/interpreter/virtual_machine.hpp>

#include <list>
#include <sstream>

#include <QMainWindow>
#include <QTextEdit>

namespace Ui
{

class InsitucCADMainWindow;

}

class InsitucCADMainWindow final
        : public QMainWindow
{

    Q_OBJECT

    std::ostringstream oss_;

    insituc::parser::parser const parser_{};
    insituc::parser::real_number_parser const real_number_parser_{};
    insituc::meta::assembler assembler_;
    insituc::meta::compiler const compiler_{assembler_};
    insituc::ast::program ast_;

    insituc::transform::derivator derive_;
    typename insituc::transform::derivator::incremental derive_incrementally_{derive_};
    typename insituc::transform::derivator::dependent_recursive derive_dependent_recursively_{derive_};

    insituc::transform::evaluator const evaluate_{};
    insituc::ast::program program_;

    insituc::runtime::instance instance_;
    insituc::runtime::translator translator_;
    insituc::runtime::virtual_machine virtual_machine_{assembler_};

public :

    explicit
    InsitucCADMainWindow(QWidget * parent = nullptr);
    ~InsitucCADMainWindow() Q_DECL_OVERRIDE;

signals :

    void globalVariableChanged(QList< QString > const &);
    void globlVariableSelectionChanged(QList< QString > const &);

private slots :

    void on_actionExit_triggered();

    void on_actionListReservedSymbols_triggered();
    void on_actionAddGlobalVariable_triggered();
    void on_actionRemoveGlobalVariable_triggered();

    void on_globalVariablesChanged();
    void on_globalVariablesSelectionChanged();

    void on_actionParse_triggered();

    void on_actionAddTransformation_triggered();
    void on_actionAddSuccessiveTransformation_triggered();
    void on_actionRemoveTransformation_triggered();

    void on_actionTransform_triggered();

    void on_actionBuild_triggered();

private :

    Ui::InsitucCADMainWindow * ui;
    TableModelGlobalVariables * tableModelGlobalVariables;
    TableModelGlobalVariables * tableModelGlobalVariablesValues;
    TableGlobalVariablesProxyModel * tableGlobalVariablesValuesProxyModel;
    EditorChooseGlobalVariable * editorChooseGlobalVariable;
    TreeModelTranfsormations * treeModelTranfsormations;

    void
    setupTextEditorMonospace(QTextEdit * textEdit);

    void
    setupTextEditorHighlighter(QTextEdit * textEdit);

    bool
    transformationTraverse(TreeModelTranfsormations::NodeList const & nodes,
                           std::list< insituc::ast::program > & _program);

};

#endif // MAINWINDOW_H
