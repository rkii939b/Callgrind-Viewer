#ifndef CALLGRINDHIGHLIGHTER_H
#define CALLGRINDHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QMap>

class CallgrindSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit CallgrindSyntaxHighlighter(QTextDocument *parent = nullptr);
    void setSearchTerm(const QString &term);  // Allows dynamic search term highlighting
    void applyFilter(const QString &filterType, double minValue, double maxValue);
protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightRule> rules;
    QVector<QString> eventOrder;  // Stores the event names (execution, call, memory, etc.)
    QString searchTerm;  // Stores the search term outside HighlightRule
    QTextCharFormat searchFormat;  // Format for highlighting search results

    QTextCharFormat executionFormat;  // Format for highlighting execution time values
    QTextCharFormat callFormat;       // Format for highlighting call count values
    QTextCharFormat memoryFormat;     // Format for highlighting memory usage values

    QString filterType_;
    double minValue_;
    double maxValue_;

    void initRules();
    double extractValueFromText(const QString &text);
};

#endif // CALLGRINDHIGHLIGHTER_H
