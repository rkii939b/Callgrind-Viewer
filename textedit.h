#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include "callgrindhighlighter.h"

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = nullptr);
    void setContents(const QString &fileName, bool enableHighlighting);
    void clearHighlighter();  // Declare the clearHighlighter() method

signals:
    void fileNameChanged(const QString &fileName);

private:
    CallgrindSyntaxHighlighter *m_highlighter = nullptr;
    QUrl srcUrl;

    QVariant loadResource(int type, const QUrl &name) override;
};

#endif // TEXTEDIT_H
