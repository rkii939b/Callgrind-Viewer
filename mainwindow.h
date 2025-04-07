#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMap>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class Assistant;
class TextEdit;

// Simple struct to hold Callgrind metrics.
struct Metrics {
    int executionTime;
    int callCount;
    int memoryUsage;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
private slots:
    void updateWindowTitle(const QString &fileName);
    void about();
    void showDocumentation();
    void open();
    void searchText();   // New search function
    void filterResults(); // New filter function



protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void createActions();
    void createMenus();
    void createSearchBar();  // UI for search & filtering

    TextEdit *textViewer;
    Assistant *assistant;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *assistantAct;
    QAction *clearAct;
    QAction *openAct;
    QAction *openCallgrindAct;  // New action for Callgrind file
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QLineEdit *searchBox;     // New search bar
    QComboBox *filterDropdown; // New filter selection
    QPushButton *searchBtn;   // New search button
};

#endif
