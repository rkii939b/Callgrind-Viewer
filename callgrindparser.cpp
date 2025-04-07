#include "callgrindparser.h"
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include <QVector>
#include <QStringList>
#include "functionstats.h"  // Include the header where FunctionStats is defined


CallgrindData parseCallgrindFile(const QString &filePath) {
    CallgrindData result;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open file" << filePath;
        return result;
    }

    QTextStream in(&file);
    bool eventsParsed = false;
    QString currentFunction;
    FunctionStats stats;  // Initialize stats here

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("events:")) {
            result.eventNames = line.mid(8).split(',', Qt::SkipEmptyParts);
            for (QString &event : result.eventNames) {
                event = event.trimmed();
            }
            eventsParsed = true;
            continue;
        }

        if (line.startsWith("fn=") && eventsParsed) {
            // Save the current function's stats if we're moving to a new function
            if (!currentFunction.isEmpty()) {
                result.functionStatsMap[currentFunction].executionTime += stats.executionTime;
                result.functionStatsMap[currentFunction].callCount += stats.callCount;
                result.functionStatsMap[currentFunction].memoryUsage += stats.memoryUsage;
            }

            currentFunction = line.mid(3).trimmed();  // Get the new function name
            stats = FunctionStats();  // Reset stats for the new function
            continue;
        }

        QStringList numbers = line.split(' ', Qt::SkipEmptyParts);
        if (!numbers.isEmpty() && numbers[0][0].isDigit()) {
            for (int i = 0; i < numbers.size() && i < result.eventNames.size(); ++i) {
                QString event = result.eventNames[i].toLower();
                int value = numbers[i].toInt();

                // Assign the value to the appropriate event (execution, call count, or memory usage)
                if (event.contains("execution")) stats.executionTime += value;
                else if (event.contains("call")) stats.callCount += value;
                else if (event.contains("memory")) stats.memoryUsage += value;
            }
        }
    }

    // Add the final function's stats
    if (!currentFunction.isEmpty()) {
        result.functionStatsMap[currentFunction].executionTime += stats.executionTime;
        result.functionStatsMap[currentFunction].callCount += stats.callCount;
        result.functionStatsMap[currentFunction].memoryUsage += stats.memoryUsage;
    }

    file.close();
    return result;
}
