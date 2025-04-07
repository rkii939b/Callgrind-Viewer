#include "assistant.h"
#include "findfiledialog.h"
#include "textedit.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>

//! [0]
FindFileDialog::FindFileDialog(TextEdit *editor, Assistant *assistant)
    : QDialog(editor)
    , currentEditor(editor)
    , currentAssistant(assistant)
{
    Q_ASSERT(currentEditor != nullptr);  // Ensure currentEditor is not null
    Q_ASSERT(currentAssistant != nullptr);  // Ensure currentAssistant is not null

    createButtons();
    createComboBoxes();
    createFilesTree();
    createLabels();
    createLayout();

    directoryComboBox->addItem(QDir::toNativeSeparators(QDir::currentPath()));
    fileNameComboBox->addItem("*");
    findFiles();

    setWindowTitle(tr("Find File"));
    //! [1]
}
//! [1]

void FindFileDialog::browse()
{
    const QString currentDirectory = directoryComboBox->currentText();
    const QString newDirectory = QFileDialog::getExistingDirectory(this,
                                                                   tr("Select Directory"), currentDirectory);
    if (!newDirectory.isEmpty()) {
        directoryComboBox->addItem(QDir::toNativeSeparators(newDirectory));
        directoryComboBox->setCurrentIndex(directoryComboBox->count() - 1);
        update();
    }
}

//! [2]
void FindFileDialog::help()
{
    currentAssistant->showDocumentation("filedialog.html");
}
//! [2]

void FindFileDialog::openFile()
{
    QTreeWidgetItem *item = foundFilesTree->currentItem();
    if (!item)
        return;

    const QString fileName = item->text(0);
    const QString path = QDir(directoryComboBox->currentText()).filePath(fileName);

    bool enableHighlighting = highlightCheckBox->isChecked();
    currentEditor->setContents(path, enableHighlighting);
    if (!highlightCheckBox->isChecked()) {
        currentEditor->clearHighlighter();
    }

    close();
}

void FindFileDialog::update()
{
    findFiles();
    buttonBox->button(QDialogButtonBox::Open)->setEnabled(foundFilesTree->topLevelItemCount() > 0);
}

void FindFileDialog::findFiles()
{
    QString wildCard = fileNameComboBox->currentText();
    if (!wildCard.endsWith('*'))
        wildCard += '*';
    const QRegularExpression filePattern(QRegularExpression::wildcardToRegularExpression(wildCard));

    const QDir directory(directoryComboBox->currentText());

    const QStringList allFiles = directory.entryList(QDir::Files | QDir::NoSymLinks);
    QStringList matchingFiles;

    for (const QString &file : allFiles) {
        if (filePattern.match(file).hasMatch())
            matchingFiles << file;
    }
    showFiles(matchingFiles);
}

void FindFileDialog::showFiles(const QStringList &files)
{
    foundFilesTree->clear();

    for (const QString &file : files) {
        QTreeWidgetItem *item = new QTreeWidgetItem(foundFilesTree, {file});

        // Highlight Callgrind files
        if (file.contains("callgrind.out") || file.endsWith(".callgrind", Qt::CaseInsensitive)) {
            item->setBackground(0, QBrush(Qt::green));// Set green highlight

                if (highlightCheckBox->isChecked() && (file.contains("callgrind.out")|| file.endsWith(".callgrind", Qt::CaseInsensitive))) {
                item->setBackground(0, QBrush(Qt::green));
            } else {
                item->setBackground(0, QBrush(Qt::transparent)); // Remove highlighting
            }
        }
    }

    if (!files.isEmpty()) {
        foundFilesTree->setCurrentItem(foundFilesTree->topLevelItem(0));
    }
}

void FindFileDialog::createButtons()
{
    browseButton = new QToolButton;
    browseButton->setText(tr("..."));
    connect(browseButton, &QAbstractButton::clicked, this, &FindFileDialog::browse);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Open
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Help);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FindFileDialog::openFile);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::helpRequested, this, &FindFileDialog::help);

    highlightCheckBox = new QCheckBox(tr("Enable Callgrind Highlighting"));
    highlightCheckBox->setChecked(true); // Default enabled
    connect(highlightCheckBox, &QCheckBox::toggled, this, &FindFileDialog::toggleHighlighting);
}

void FindFileDialog::createComboBoxes()
{
    directoryComboBox = new QComboBox;
    fileNameComboBox = new QComboBox;

    fileNameComboBox->setEditable(true);
    fileNameComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    directoryComboBox->setMinimumContentsLength(30);
    directoryComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    directoryComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    connect(fileNameComboBox, &QComboBox::editTextChanged, this, &FindFileDialog::update);
    connect(directoryComboBox, &QComboBox::currentTextChanged, this, &FindFileDialog::update);
}

void FindFileDialog::createFilesTree()
{
    foundFilesTree = new QTreeWidget;
    foundFilesTree->setColumnCount(1);
    foundFilesTree->setHeaderLabels(QStringList(tr("Matching Files")));
    foundFilesTree->setRootIsDecorated(false);
    foundFilesTree->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(foundFilesTree, &QTreeWidget::itemActivated, this, &FindFileDialog::openFile);
}

void FindFileDialog::createLabels()
{
    directoryLabel = new QLabel(tr("Search in:"));
    fileNameLabel = new QLabel(tr("File name (including wildcards):"));
}

void FindFileDialog::createLayout()
{
    QHBoxLayout *fileLayout = new QHBoxLayout;
    fileLayout->addWidget(fileNameLabel);
    fileLayout->addWidget(fileNameComboBox);

    QHBoxLayout *directoryLayout = new QHBoxLayout;
    directoryLayout->addWidget(directoryLabel);
    directoryLayout->addWidget(directoryComboBox);
    directoryLayout->addWidget(browseButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(fileLayout);
    mainLayout->addLayout(directoryLayout);
    mainLayout->addWidget(foundFilesTree);
    mainLayout->addWidget(highlightCheckBox);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}
void FindFileDialog::toggleHighlighting()
{
    update(); // Refresh the file list when the checkbox is toggled
}


QString FindFileDialog::selectedFile() const
{
    QTreeWidgetItem *item = foundFilesTree->currentItem();
    if (!item)
        return QString();  // Return an empty string if no file is selected.

    const QString fileName = item->text(0);
    return QDir(directoryComboBox->currentText()).filePath(fileName);
}
