#include "callgrindhighlighter.h"
#include <QDebug>

CallgrindSyntaxHighlighter::CallgrindSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    initRules();
}

void CallgrindSyntaxHighlighter::initRules()
{
    // Comments (green)
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression(R"(^#.*$)");
    commentRule.format = commentFormat;
    rules.append(commentRule);

    // Commands (blue)
    QTextCharFormat commandFormat;
    commandFormat.setForeground(Qt::blue);
    HighlightRule commandRule;
    commandRule.pattern = QRegularExpression(R"(^(cmd|events|creator|pid|desc|positions|summary|version):)");
    commandRule.format = commandFormat;
    rules.append(commandRule);

    // Positions (magenta)
    QTextCharFormat positionFormat;
    positionFormat.setForeground(Qt::darkMagenta);
    HighlightRule positionRule;
    positionRule.pattern = QRegularExpression(R"(\b(fl|fn|ob|cfi|cfn|cob|jump)=)");
    positionRule.format = positionFormat;
    rules.append(positionRule);

    // Numbers (red)
    QTextCharFormat numberFormat;
    numberFormat.setForeground(Qt::red);
    HighlightRule numberRule;
    numberRule.pattern = QRegularExpression(R"(\b\d+\b)");
    numberRule.format = numberFormat;
    rules.append(numberRule);

    // Hex addresses (cyan)
    QTextCharFormat hexFormat;
    hexFormat.setForeground(Qt::cyan);
    HighlightRule hexRule;
    hexRule.pattern = QRegularExpression(R"(0x[0-9A-Fa-f]+)");
    hexRule.format = hexFormat;
    rules.append(hexRule);
}

void CallgrindSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightRule &rule : rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
