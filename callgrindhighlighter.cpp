#include "callgrindhighlighter.h"
#include <QRegularExpressionMatchIterator>
#include <QDebug>


CallgrindSyntaxHighlighter::CallgrindSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    initRules();

    // Search term formatting (highlight in yellow)
    searchFormat.setBackground(Qt::yellow);
    searchFormat.setForeground(Qt::black);
    searchTerm.clear();

    eventOrder.clear();
}

void CallgrindSyntaxHighlighter::setSearchTerm(const QString &term)
{
    searchTerm = term;
    rehighlight();  // Force rehighlighting when the search term changes
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

    // Hex addresses (cyan)
    QTextCharFormat hexFormat;
    hexFormat.setForeground(Qt::cyan);
    HighlightRule hexRule;
    hexRule.pattern = QRegularExpression(R"(0x[0-9A-Fa-f]+)");
    hexRule.format = hexFormat;
    rules.append(hexRule);

    // Custom format colors for values by event
    executionFormat.setForeground(Qt::blue);
    callFormat.setForeground(Qt::magenta);
    memoryFormat.setForeground(Qt::darkGreen);
}

void CallgrindSyntaxHighlighter::applyFilter(const QString &filterType, double minValue, double maxValue)
{
    filterType_ = filterType;
    minValue_ = minValue;
    maxValue_ = maxValue;

    rehighlight();  // Refresh the highlights based on the new filter
}

void CallgrindSyntaxHighlighter::highlightBlock(const QString &text)
{
    // Parse event names from the "events:" line and store the order
    if (text.startsWith("events:")) {
        QStringList parts = text.mid(7).split(',', Qt::SkipEmptyParts);
        eventOrder.clear();
        for (QString &part : parts) {
            eventOrder.append(part.trimmed().toLower());
        }
    }

    // Apply general syntax highlighting for non-filter-related patterns
    for (const HighlightRule &rule : rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Custom highlighting for number lines
    QRegularExpression numberLine(R"(^\s*(\d+\s+)+\d+\s*$)");
    if (numberLine.match(text).hasMatch() && !eventOrder.isEmpty()) {
        QStringList numbers = text.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
        int pos = 0;
        for (int i = 0; i < numbers.size() && i < eventOrder.size(); ++i) {
            QString value = numbers[i];
            int startIndex = text.indexOf(value, pos);
            int length = value.length();

            QString event = eventOrder[i];
            if (event.contains("execution")) {
                setFormat(startIndex, length, executionFormat);
            } else if (event.contains("call")) {
                setFormat(startIndex, length, callFormat);
            } else if (event.contains("memory")) {
                setFormat(startIndex, length, memoryFormat);
            }

            pos = startIndex + length;
        }
    }

    // Extract numeric value based on filter type
    double value = extractValueFromText(text);

    // Apply highlighting based on the filter type and value range
    if (filterType_ == "execution_time" && value >= minValue_ && value <= maxValue_) {
        setFormat(0, text.length(), Qt::green);  // Green for execution time
    } else if (filterType_ == "call_count" && value >= minValue_ && value <= maxValue_) {
        setFormat(0, text.length(), Qt::yellow);  // Yellow for call count
    } else if (filterType_ == "memory_usage" && value >= minValue_ && value <= maxValue_) {
        setFormat(0, text.length(), Qt::blue);  // Blue for memory usage
    } else {
        setFormat(0, text.length(), Qt::transparent);  // No highlight if it doesn't match
    }

    // Highlight search terms (yellow background for the search term)
    if (!searchTerm.isEmpty()) {
        QRegularExpression searchPattern(QRegularExpression::escape(searchTerm), QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator result = searchPattern.globalMatch(text);
        while (result.hasNext()) {
            QRegularExpressionMatch match = result.next();
            setFormat(match.capturedStart(), match.capturedLength(), searchFormat);
        }
    }
}

double CallgrindSyntaxHighlighter::extractValueFromText(const QString &text)
{
    // Match only lines with numbers corresponding to events
    QRegularExpression numberLine(R"(^\s*(\d+\s+)+\d+\s*$)");
    if (numberLine.match(text).hasMatch() && !eventOrder.isEmpty()) {
        QStringList numbers = text.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);

        for (int i = 0; i < numbers.size() && i < eventOrder.size(); ++i) {
            QString event = eventOrder[i].toLower();
            double value = numbers[i].toDouble();

            if (filterType_ == "execution_time" && event.contains("exec")) {
                return value;
            }
            if (filterType_ == "call_count" && event.contains("call")) {
                return value;
            }
            if (filterType_ == "memory_usage" && event.contains("mem")) {
                return value;
            }
        }
    }
    return 0.0;
}
