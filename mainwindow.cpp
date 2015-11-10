#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sourcesyntaxhighlighter.h"
#include "texttermformatting.h"
#include "settings.h"

//#include <modeltest.h>

#include <insituc/ast/io.hpp>
#include <insituc/meta/io.hpp>

#include <QMessageBox>
#include <QModelIndexList>
#include <QDebug>

#include <algorithm>
#include <utility>
#include <iostream>

#include <cassert>

using namespace insituc;

void
InsitucCADMainWindow::setupTextEditorMonospace(QTextEdit * textEdit)
{
    QTextDocument * document = textEdit->document();
    QFont font = document->defaultFont();
    font.setFamily("monospace");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    QFontMetrics metrics(font);
    //Q_ASSERT(QFontInfo(font).fixedPitch());
    document->setDefaultFont(font);
    const int tabStop = 4;
    textEdit->setTabStopWidth(tabStop * metrics.width(' '));
}

void
InsitucCADMainWindow::setupTextEditorHighlighter(QTextEdit * textEdit)
{
    auto * const syntaxHighlighterSource = new SourceSyntaxHighlighter(textEdit->document());
    connect(this,
            SIGNAL(globlVariableSelectionChanged(QList< QString > const &)),
            syntaxHighlighterSource,
            SLOT(on_globalVariablesSelectionChanged(QList< QString > const &)));
    connect(this,
            SIGNAL(globalVariableChanged(QList< QString > const &)),
            syntaxHighlighterSource,
            SLOT(on_globalVariablesChanged(QList< QString > const &)));
}

InsitucCADMainWindow::InsitucCADMainWindow(QWidget * parent)
    : QMainWindow(parent)
    , ui(new Ui::InsitucCADMainWindow)
{
    ui->setupUi(this);
    tableModelGlobalVariables = new TableModelGlobalVariables(this);
    tableModelGlobalVariablesValues = new TableModelGlobalVariables(this);
    tableGlobalVariablesValuesProxyModel = new TableGlobalVariablesProxyModel(this);
    tableGlobalVariablesValuesProxyModel->setSourceModel(tableModelGlobalVariablesValues);
    ui->tableViewGlobalVariables->setModel(tableModelGlobalVariables);
    ui->tableViewGlobalVariablesValues->setModel(tableGlobalVariablesValuesProxyModel);
    ui->tableViewGlobalVariables->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewGlobalVariables->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableViewGlobalVariablesValues->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewGlobalVariablesValues->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    {
        connect(tableModelGlobalVariables,
                SIGNAL(rowsInserted(QModelIndex const &, int, int)),
                SLOT(on_globalVariablesChanged()));
        connect(tableModelGlobalVariables,
                SIGNAL(rowsRemoved(QModelIndex const &, int, int)),
                SLOT(on_globalVariablesChanged()));
        connect(tableModelGlobalVariables,
                SIGNAL(dataChanged(QModelIndex const &, QModelIndex const &, QVector< int > const &)),
                SLOT(on_globalVariablesChanged()));
        connect(tableModelGlobalVariables,
                SIGNAL(modelReset()),
                SLOT(on_globalVariablesChanged()));
    }
    connect(ui->tableViewGlobalVariables->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection const &, QItemSelection const &)),
            SLOT(on_globalVariablesSelectionChanged()));
    {
        treeModelTranfsormations = new TreeModelTranfsormations(this);
        //delete new ModelTest(treeModelTranfsormations, this);
        ui->treeViewTransformations->setModel(treeModelTranfsormations);
        editorChooseGlobalVariable = new EditorChooseGlobalVariable(this);
        ui->treeViewTransformations->setItemDelegateForColumn(0, editorChooseGlobalVariable);

        ui->toolButtonAddTransformation->setDefaultAction(ui->actionAddTransformation);
        ui->toolButtonAddChildTransformation->setDefaultAction(ui->actionAddSuccessiveTransformation);
        ui->toolButtonRemoveTransformation->setDefaultAction(ui->actionRemoveTransformation);
        ui->treeViewTransformations->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    setupTextEditorMonospace(ui->textEditSource);
    setupTextEditorHighlighter(ui->textEditSource);
    setupTextEditorMonospace(ui->textEditAst);
    setupTextEditorHighlighter(ui->textEditAst);
    setupTextEditorMonospace(ui->textEditAssembly);
    ui->toolButtonAddGlobalVariable->setDefaultAction(ui->actionAddGlobalVariable);
    ui->toolButtonRemoveGlobalVariable->setDefaultAction(ui->actionRemoveGlobalVariable);
    ui->toolButtonParse->setDefaultAction(ui->actionParse);
    ui->toolButtonTransform->setDefaultAction(ui->actionTransform);
    ui->toolButtonBuild->setDefaultAction(ui->actionBuild);
    ui->toolButtonInterpret->setDefaultAction(ui->actionInterpret);
    ui->toolButtonCompile->setDefaultAction(ui->actionCompile);
    {
        QPalette termPalette = ui->textEditAssembly->palette();
        termPalette.setColor(QPalette::Base, Qt::black);
        termPalette.setColor(QPalette::Text, Qt::white);
        ui->textEditAssembly->setPalette(termPalette);
    }
    ui->tabWidget->setCurrentIndex(0);
    ui->textEditSource->setFocus();

    ui->textEditSource->setPlainText("function Gauss(x)\n\treturn a * pow2(-sqr(r * (x / m - one)))\nend");
    QVector< QPair< QString, QString > > globalVariables;
    globalVariables.append(QPair< QString, QString >("a", "1"));
    globalVariables.append(QPair< QString, QString >("r", "1"));
    globalVariables.append(QPair< QString, QString >("m", "0"));
    tableModelGlobalVariables->populateWithData(globalVariables);
}

InsitucCADMainWindow::~InsitucCADMainWindow()
{
    delete ui;
}

void InsitucCADMainWindow::on_actionAddGlobalVariable_triggered()
{
    auto * const selectionModel = ui->tableViewGlobalVariables->selectionModel();
    QModelIndex index = selectionModel->currentIndex();
    if (!tableModelGlobalVariables->insertRow(index.row())) {
        qWarning() << "Can't add global variable";
    }
}

void InsitucCADMainWindow::on_actionRemoveGlobalVariable_triggered()
{
    QModelIndexList indexList = ui->tableViewGlobalVariables->selectionModel()->selectedRows();
    QList< QPersistentModelIndex > persistentIdexList;
    for (QModelIndex const & index : indexList) {
        persistentIdexList.append(index);
    }
    for (QPersistentModelIndex const & persistentIndex : persistentIdexList) {
        if (!tableModelGlobalVariables->removeRow(persistentIndex.row())) {
            qWarning() << "Can't remove global variable";
        }
    }
}

void InsitucCADMainWindow::on_actionListReservedSymbols_triggered()
{
    QStringList reservedSymbols = Settings::getPersistentSettings().reservedSymbols;
    reservedSymbols.sort();
    QMessageBox::information(this, "Reserved symbols list", reservedSymbols.join('\t'));
}

void InsitucCADMainWindow::on_globalVariablesChanged()
{
    QVector< QPair< QString, QString > > const & globalVariables = tableModelGlobalVariables->globalVariables;
    QList< QString > globalVariablesNames;
    for (QPair< QString, QString > const & globalVariable : globalVariables) {
        globalVariablesNames.append(globalVariable.first);
    }
    emit globalVariableChanged(globalVariablesNames);
}

void InsitucCADMainWindow::on_globalVariablesSelectionChanged()
{
    QModelIndexList indexes = ui->tableViewGlobalVariables->selectionModel()->selectedIndexes();
    QVector< QPair< QString, QString > > const & globalVariables = tableModelGlobalVariables->globalVariables;
    QList< QString > selectedGlobalVariablesNames;
    QListIterator< QModelIndex > i(indexes);
    while (i.hasNext()) {
        selectedGlobalVariablesNames.append(globalVariables[i.next().row()].first);
    }
    emit globlVariableSelectionChanged(selectedGlobalVariablesNames);
}

void InsitucCADMainWindow::on_actionExit_triggered()
{
    close();
}

void InsitucCADMainWindow::on_actionParse_triggered()
{
    string_type const source_ = ui->textEditSource->toPlainText().toStdString();
    auto const parse_result_ = parser_(std::cbegin(source_), std::cend(source_));
    if (!!parse_result_.error_description_) {
        std::cerr << "Can't parse the source text:" << std::endl
                  << "//< begin" << std::endl
                  << source_ << std::endl
                  << "//< end" << std::endl;
        QMessageBox::critical(this, "Failure!", "Can't parse the source text.");
        return;
    }
    ast_ = std::move(parse_result_.ast_);
    {
        int size = tableModelGlobalVariables->globalVariables.size();
        for (int i = 0; i < size; ++i) {
            QPair< QString, QString > const & globalVariable = tableModelGlobalVariables->globalVariables.at(i);
            if (globalVariable.first.isEmpty()) {
                QMessageBox::critical(this, "Global variables", "Global variables list contains undefined global variable name at line " + QString::number(i));
                return;
            }
        }
        treeModelTranfsormations->setWrts(tableModelGlobalVariables->globalVariables);
    }
    QMessageBox::information(this, "Success!", "Source text successfully parsed resulting in AST.");
}

void InsitucCADMainWindow::on_actionAddTransformation_triggered()
{
    auto * const selectionModel = ui->treeViewTransformations->selectionModel();
    QModelIndex index = selectionModel->currentIndex();
    if (index.isValid()) {
        if (!treeModelTranfsormations->insertRow(index.row(), index.parent())) {
            qWarning() << "Can't add transformation";
            QMessageBox::critical(this, "Failure!", "Can't add transformation.");
            return;
        }
    } else {
        if (!treeModelTranfsormations->insertRow(-1)) {
            qWarning() << "Can't add transformation";
            QMessageBox::critical(this, "Failure!", "Can't add transformation.");
            return;
        }
    }
}

void InsitucCADMainWindow::on_actionAddSuccessiveTransformation_triggered()
{
    auto * const selectionModel = ui->treeViewTransformations->selectionModel();
    QModelIndex index = selectionModel->currentIndex();
    QModelIndex parentIndex = treeModelTranfsormations->index(index.row(), 0, index.parent());
    int position = treeModelTranfsormations->rowCount(parentIndex);
    if (!treeModelTranfsormations->insertRow(position, parentIndex)) {
        qWarning() << "Can't add subsequent transformation";
        QMessageBox::critical(this, "Failure!", "Can't add subsequent transformation.");
        return;
    }
    ui->treeViewTransformations->expand(parentIndex);
}

void InsitucCADMainWindow::on_actionRemoveTransformation_triggered()
{
    QModelIndexList indexList = ui->treeViewTransformations->selectionModel()->selectedRows();
    QList< QPersistentModelIndex > persistentIndexList;
    for (QModelIndex const & index : indexList) {
        persistentIndexList.append(index);
    }
    for (QPersistentModelIndex const & persistentIndex : persistentIndexList) {
        if (persistentIndex.isValid()) {
            if (!treeModelTranfsormations->removeRow(persistentIndex.row(), persistentIndex.parent())) {
                qWarning() << "Can't remove transformation";
            }
        }
    }
}

bool
InsitucCADMainWindow::transformationTraverse(TreeModelTranfsormations::NodeList const & nodes,
                                             std::list< insituc::ast::program > & _program)
{
    ast::program const & node_ = _program.back();
    for (std::shared_ptr< TreeModelTranfsormations::Node > const & pnode : nodes) {
        TreeModelTranfsormations::Node const & node = *pnode;
        if (node.used) {
            if (!(node.wrt < treeModelTranfsormations->wrts.size())) {
                qWarning("wrt index is greater then number of wrts");
                return false;
            }
            if (node.wrt < 0) {
                qWarning("wrt index is negative");
                return false;
            }
            derive_incrementally_.enter();
            ast::symbol const wrt_{treeModelTranfsormations->wrts.at(node.wrt).first.toStdString()};
            derive_incrementally_.push(node_, wrt_);
            if (node.postSimplify) {
                derive_incrementally_.derive(evaluate_);
            } else {
                derive_incrementally_.derive([] (auto && p) { return std::move(p); });
            }
            derive_incrementally_.pop();
            _program.push_back(derive_incrementally_.leave());
            if (!transformationTraverse(node.children, _program)) {
                return false;
            }
        }
    }
    return true;
}

void InsitucCADMainWindow::on_actionTransform_triggered()
{
    std::list< ast::program > sources_;
    sources_.push_back(evaluate_(ast_));
    if (!transformationTraverse(treeModelTranfsormations->nodes, sources_)) {
        qWarning("can't apply transformations to AST");
        QMessageBox::critical(this, "Failure!", "Can't apply transformations to AST.");
        return;
    }
    program_.entries_.clear();
    for (ast::program & source_ : sources_) {
        program_.append(std::move(source_));
    }
    {
        oss_ << program_ << std::endl;
        ui->textEditAst->setPlainText(QString::fromStdString(oss_.str()));
        oss_.clear();
        oss_.str("");
    }
    QMessageBox::information(this, "Success!", "AST successfully transformed.");
}

void InsitucCADMainWindow::on_actionBuild_triggered()
{
    if (program_.entries_.empty()) {
        std::cerr << "Empty AST." << std::endl;
        QMessageBox::critical(this, "Failure!", "AST is empty.");
        return;
    }
    if (!assembler_.clear()) {
        std::cerr << "Can't clear the assembler istance" << std::endl;
        QMessageBox::critical(this, "Failure!", "Can't clear the assembler istance");
        return;
    }
    {
        int size = treeModelTranfsormations->wrts.size();
        for (int i = 0; i < size; ++i) {
            QPair< QString, QString > const & globalVariable = treeModelTranfsormations->wrts.at(i);
            Q_ASSERT(!globalVariable.first.isEmpty());
            meta::symbol_type const name_{{globalVariable.first.toStdString()}, {}, {}};
            assert(!assembler_.is_global_variable(name_));
            assert(!assembler_.is_reserved_symbol(name_));
            assert(!assembler_.is_dummy_placeholder(name_));
            assert(!assembler_.is_function(name_));
            if (globalVariable.second.isEmpty()) {
                if (!assembler_.add_global_variable(name_)) {
                    oss_ << "Variable name \"" << name_ << "\" at line " << i << " is not valid global variable name";
                    std::string const s = oss_.str();
                    std::cerr << s << std::endl;
                    QMessageBox::critical(this, "Failure!", QString::fromStdString(s));
                    oss_.clear();
                    oss_.str("");
                    return;
                }
                std::cout << "Variable \"" << name_ << "\" without initial value" << std::endl;
            } else {
                meta::symbol_type const value_string_{{globalVariable.second.toStdString()}, {}, {}};
                auto value_ = real_number_parser_(std::cbegin(value_string_.symbol_.name_), std::cend(value_string_.symbol_.name_));
                if (!value_) {                    
                    oss_ << "String \"" << value_string_ << "\" at line " << i << " is not valid global variable value representation";
                    std::string const s = oss_.str();
                    std::cerr << s << std::endl;
                    QMessageBox::critical(this, "Failure!", QString::fromStdString(s));
                    oss_.clear();
                    oss_.str("");
                    return;
                }
                if (!assembler_.add_global_variable(name_, value_.value())) {
                    oss_ << "Variable name \"" << name_ << "\" at line " << i << " is not valid global variable name";
                    std::string const s = oss_.str();
                    std::cerr << s << std::endl;
                    QMessageBox::critical(this, "Failure!", QString::fromStdString(s));
                    oss_.clear();
                    oss_.str("");
                    return;
                }
                std::cout << "Variable \"" << name_ << "\" with initial value " << value_.value() << std::endl;
            }
        }
    }
    if (!compiler_(program_)) {
        std::cerr << "Can't compile AST:" << std::endl
                  << "//< begin" << std::endl
                  << program_ << std::endl
                  << "//< end" << std::endl;
        QMessageBox::critical(this, "Failure!", "Can't compile AST.");
        return;
    }
    {
        oss_ << assembler_;
        QTextDocument * document = ui->textEditAssembly->document();
        document->clear();
        QTextCursor cursor = setTextTermFormatting(document, QString::fromStdString(oss_.str()));
        cursor.movePosition(QTextCursor::Start);
        ui->textEditAssembly->setTextCursor(cursor);
        oss_.clear();
        oss_.str("");
    }
    tableModelGlobalVariablesValues->populateWithData(treeModelTranfsormations->wrts);
    QMessageBox::information(this, "Success!", "AST successfully compiled.");
}
