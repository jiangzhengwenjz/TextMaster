#ifndef READ_CHARMAP_H
#define READ_CHARMAP_H

#include <QtGlobal>
#include <QHash>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QStringList>

class CharMapReader
{
private:
    bool success;

public:
    QHash<QString, QString> *charMapDict;
    QHash<QString, QString> *charMapDictRev;
    explicit CharMapReader(const QString *fileName = Q_NULLPTR);
    virtual ~CharMapReader();
    bool State() const noexcept;
};

#endif // READ_CHARMAP_H
