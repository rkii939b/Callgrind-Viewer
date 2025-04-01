#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class Assistant;
class TextEdit;

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

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void createActions();
    void createMenus();

    TextEdit *textViewer;
    Assistant *assistant;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *assistantAct;
    QAction *clearAct;
    QAction *openAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
