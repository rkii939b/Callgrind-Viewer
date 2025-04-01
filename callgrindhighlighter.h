#ifndef CALLGRINDHIGHLIGHTER_H
#define CALLGRINDHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class CallgrindSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit CallgrindSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> rules;

    void initRules();
};

#endif // CALLGRINDHIGHLIGHTER_H
