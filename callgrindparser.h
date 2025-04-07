#ifndef CALLGRINDPARSER_H
#define CALLGRINDPARSER_H

#include <QString>
#include <QMap>
#include <QStringList>
#include "functionstats.h" //  Added to include FunctionStats


struct CallgrindData {

    QMap<QString, FunctionStats> functionStatsMap; //  Aggregated stats map
    QStringList eventNames;
};

CallgrindData parseCallgrindFile(const QString &filePath); //  Updated return type

#endif // CALLGRINDPARSER_H
