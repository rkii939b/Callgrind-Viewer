#include "textedit.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent), m_highlighter(nullptr)
{
    setReadOnly(true);
}

void TextEdit::setContents(const QString &fileName, bool enableHighlighting)
{
    // Clear existing content and highlighter
    clear();
    if (m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;
    }

    QFileInfo fi(fileName);
    srcUrl = QUrl::fromLocalFile(fi.absoluteFilePath());

    if (enableHighlighting && (fileName.endsWith(".callgrind", Qt::CaseInsensitive) ||
                               fileName.contains("callgrind.out"))) {
        qDebug() << "Initializing Callgrind highlighter for:" << fileName;
        m_highlighter = new CallgrindSyntaxHighlighter(document());
    }

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        if (fileName.endsWith(".html")) {
            setHtml(content);
        } else {
            setPlainText(content);
        }
    }

    emit fileNameChanged(fileName);
}

void TextEdit::clearHighlighter()
{
    // Delete the highlighter if it exists
    if (m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;
    }
}

QVariant TextEdit::loadResource(int type, const QUrl &name)
{
    if (type == QTextDocument::ImageResource) {
        QFile file(srcUrl.resolved(name).toLocalFile());
        if (file.open(QIODevice::ReadOnly))
            return file.readAll();
    }
    return QTextEdit::loadResource(type, name);
}