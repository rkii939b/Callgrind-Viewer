#include "assistant.h"
#include "findfiledialog.h"
#include "mainwindow.h"
#include "textedit.h"
#include "callgrindparser.h"  // Include the new header

#include <QAction>
#include <QApplication>
#include <QLibraryInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

// search & filter
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

using namespace Qt::StringLiterals;

// ![0]
MainWindow::MainWindow()
    : textViewer(new TextEdit)
    , assistant(new Assistant)
{
    textViewer->setContents(QLibraryInfo::path(QLibraryInfo::ExamplesPath)
                            + "/assistant/simpletextviewer/documentation/intro.html"_L1, false);
    setCentralWidget(textViewer);

    createActions();
    createMenus();
    createSearchBar(); // New function to initialize search UI

    setWindowTitle(tr("Simple Text Viewer"));
    resize(750, 400);

    connect(textViewer, &TextEdit::fileNameChanged, this, &MainWindow::updateWindowTitle);
}
//! [1]

void MainWindow::closeEvent(QCloseEvent *)
{
    delete assistant;
}

void MainWindow::updateWindowTitle(const QString &fileName)
{
    setWindowTitle(tr("Simple Text Viewer - %1").arg(fileName));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Text Viewer"),
                       tr("This example demonstrates how to use\n"
                          "Qt Assistant as help system for your own application \n"
                          "and it can also detect Callgrind files and highlight them."));
}

void MainWindow::showDocumentation()
{
    assistant->showDocumentation("index.html");
}

// Integrated open() slot.
// If the selected file appears to be a Callgrind file (by extension or file name),
// it will be parsed and aggregated output is displayed in the TextEdit widget.
// Otherwise, it falls back to the existing FindFileDialog.
void MainWindow::open()
{
    FindFileDialog dialog(textViewer, assistant);
    if (dialog.exec() != QDialog::Accepted)
        return;

    // Retrieve the selected file path from the dialog.
    QString filePath = dialog.selectedFile();
    if (filePath.isEmpty())
        return;  // No file chosen

    // Check if the file is a Callgrind file
    if (filePath.endsWith(".callgrind", Qt::CaseInsensitive)) {
        CallgrindData callgrindData = parseCallgrindFile(filePath);

        // Format the output for display in TextEdit
        QString outputText;
        QMapIterator<QString, FunctionStats> i(callgrindData.functionStatsMap);
        while (i.hasNext()) {
            i.next();
            QString functionName = i.key();
            FunctionStats funcStats = i.value();

            outputText += "Function: " + functionName + "\n";
            if (callgrindData.eventNames.size() > 0) outputText += "  " + callgrindData.eventNames[0] + ": " + QString::number(funcStats.executionTime) + "\n";
            if (callgrindData.eventNames.size() > 1) outputText += "  " + callgrindData.eventNames[1] + ": " + QString::number(funcStats.callCount) + "\n";
            if (callgrindData.eventNames.size() > 2) outputText += "  " + callgrindData.eventNames[2] + ": " + QString::number(funcStats.memoryUsage) + "\n\n";
        }

        textViewer->setPlainText(outputText); // Display parsed results
        textViewer->clearHighlighter(); // clear the highlighter if it exist.


    } else {
        // If not a Callgrind file, use the existing logic
        textViewer->setContents(filePath, true);
    }
}


void MainWindow::createActions()
{
    assistantAct = new QAction(tr("Help Contents"), this);
    assistantAct->setShortcut(QKeySequence::HelpContents);
    connect(assistantAct, &QAction::triggered, this, &MainWindow::showDocumentation);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    clearAct = new QAction(tr("&Clear"), this);
    clearAct->setShortcut(tr("Ctrl+C"));
    connect(clearAct, &QAction::triggered, textViewer, &QTextEdit::clear);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, &QAction::triggered, QApplication::aboutQt);
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(clearAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(assistantAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::createSearchBar()
{
    QWidget *searchWidget = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);

    searchBox = new QLineEdit(this);
    searchBtn = new QPushButton(tr("Search"), this);
    filterDropdown = new QComboBox(this);

    filterDropdown->addItem(tr("All"));
    filterDropdown->addItem(tr("Execution Time"));
    filterDropdown->addItem(tr("Call Count"));
    filterDropdown->addItem(tr("Memory Usage"));

    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(searchBtn);
    searchLayout->addWidget(filterDropdown);

    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::searchText);
    connect(filterDropdown, &QComboBox::currentTextChanged, this, &MainWindow::filterResults);

    menuBar()->setCornerWidget(searchWidget, Qt::TopRightCorner);
}

void MainWindow::searchText()
{
    QString searchTerm = searchBox->text();
    if (searchTerm.isEmpty())
        return;

    textViewer->find(searchTerm); // Use built-in find() of QTextEdit
}

void MainWindow::filterResults()
{
    QString filterOption = filterDropdown->currentText();
    textViewer->applyFilter(filterOption);
}
