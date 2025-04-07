#include "textedit.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "callgrindparser.h"  // Include the parser
#include "functionstats.h"    // Include the FunctionStats struct

TextEdit::TextEdit(QWidget *parent)
    : QTextEdit(parent), m_highlighter(nullptr)
{
    setReadOnly(true);
}

void TextEdit::setContents(const QString &fileName, bool enableHighlighting)
{
    // Clear previous highlighter if any
    if (m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;
    }

    QFileInfo fi(fileName);
    srcUrl = QUrl::fromLocalFile(fi.absoluteFilePath());  // Store the file's URL

    // Optionally enable syntax highlighting for Callgrind files
    if (enableHighlighting && (fileName.endsWith(".callgrind", Qt::CaseInsensitive) ||
                               fileName.endsWith("callgrind.out", Qt::CaseInsensitive))) {
        qDebug() << "Initializing Callgrind highlighter for:" << fileName;
        m_highlighter = new CallgrindSyntaxHighlighter(document());
    }

    // Clear previous function data
    functionData.clear();

    // Read the content of the file into originalContent
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file for reading:" << fileName;
        return;
    }
    QTextStream in(&file);
    originalContent = in.readAll();  // Store the entire file content

    // Set the original content in the QTextEdit as plain text
    setPlainText(originalContent);  // This will display the full content without any filtering

    // Parse the content and get the aggregated Callgrind data
    CallgrindData parsedData = parseCallgrindFile(fileName);

    // Aggregate function stats into functionData
    for (auto it = parsedData.functionStatsMap.begin(); it != parsedData.functionStatsMap.end(); ++it) {
        QString functionName = it.key();
        FunctionStats stats = it.value();

        // Directly access the value using operator[] and update it
        FunctionStats &existingStats = functionData[functionName];

        // Update the stats directly
        existingStats.executionTime += stats.executionTime;
        existingStats.callCount += stats.callCount;
        existingStats.memoryUsage += stats.memoryUsage;
    }

    // Apply the filter to update the displayed content
    applyFilter("All");  // Example: filtering by execution time or function statistics
}





void TextEdit::clearHighlighter()
{
    if (m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;  // Prevent dangling pointer
    }
}



void TextEdit::applyFilter(const QString &filterType)
{
    QString displayText;  // This will hold the filtered text to display

    for (auto it = functionData.constBegin(); it != functionData.constEnd(); ++it) {
        QString functionName = it.key();
        FunctionStats stats = it.value();

        int executionTime = stats.executionTime;
        int callCount = stats.callCount;
        int memoryUsage = stats.memoryUsage;

        if (filterType == "All") {
            displayText += "<b>Function Name:</b> " + functionName + "<br>";
            displayText += "Execution Time: <font color='blue'>" + QString::number(executionTime) + "</font><br>";
            displayText += "Call Count: <font color='magenta'>" + QString::number(callCount) + "</font><br>";
            displayText += "Memory Usage: <font color='darkgreen'>" + QString::number(memoryUsage) + "</font><br><br>";
        }
        else if (filterType == "Execution Time") {
            displayText += "<b>Function Name:</b> " + functionName + "<br>";
            displayText += "Execution Time: <font color='blue'>" + QString::number(executionTime) + "</font><br><br>";
        }
        else if (filterType == "Call Count") {
            displayText += "<b>Function Name:</b> " + functionName + "<br>";
            displayText += "Call Count: <font color='magenta'>" + QString::number(callCount) + "</font><br><br>";
        }
        else if (filterType == "Memory Usage") {
            displayText += "<b>Function Name:</b> " + functionName + "<br>";
            displayText += "Memory Usage: <font color='darkgreen'>" + QString::number(memoryUsage) + "</font><br><br>";
        }
    }

    // If filtered data is found, display it. Otherwise, show a "No results found" message.
    if (!displayText.isEmpty()) {
        setHtml(displayText);  // Use HTML formatting for color highlighting
    } else {
        setPlainText("No results found.");  // Display message if no matching data
    }

    // Optional: Apply additional syntax highlighting for filtered data
    if (m_highlighter) {
        if (filterType == "Execution Time") {
            m_highlighter->applyFilter("execution_time", 1000, INT_MAX); // Highlight execution time > 1000
        } else if (filterType == "Call Count") {
            m_highlighter->applyFilter("call_count", 10000, INT_MAX); // Highlight call count > 10000
        } else if (filterType == "Memory Usage") {
            m_highlighter->applyFilter("memory_usage", 5000, INT_MAX); // Highlight memory usage > 5000
        }
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

void TextEdit::setFunctionData(const QMap<QString, FunctionStats> &data)
{
    functionData = data;  // Correctly update the functionData member
}

