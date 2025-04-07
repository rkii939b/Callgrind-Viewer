#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QMap>
#include <QVector>
#include <QUrl>
#include "callgrindhighlighter.h"
#include "functionstats.h" //Include aggregated data struct

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = nullptr);
    void setContents(const QString &fileName, bool enableHighlighting);


    void clearHighlighter();  // Declare the clearHighlighter() method
    void setFunctionData(const QMap<QString, FunctionStats> &data);
    void setEventNames(const QStringList &names);
    void setOriginalContent(const QString &content);
    void applyFilter(const QString &filterType);
signals:
    void fileNameChanged(const QString &fileName);

private:
    CallgrindSyntaxHighlighter *m_highlighter = nullptr;
    QUrl srcUrl;

    QVariant loadResource(int type, const QUrl &name) override;
    QString originalContent;  // Stores the full content of the file
    QStringList eventNames;
    QMap<QString, FunctionStats> functionData;    //dynamic filtering
};

#endif // TEXTEDIT_H
