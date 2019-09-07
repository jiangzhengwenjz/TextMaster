#include "core/read_charmap.h"

/*
 * Nested linked list is also okay, but it's probably not worth it
 * because most characters are just 1 or 2 bytes, so a hashmap is really
 * good here. 
 */

CharMapReader::CharMapReader(const QString *fileName) :
    success(false),
    charMapDict(new QHash<QString, QString>),
    charMapDictRev(new QHash<QString, QString>)
{
    if (!fileName->isNull() && !fileName->isEmpty() && charMapDict && charMapDictRev)
    {
        QFile inputFile{*fileName};
        if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString line{};
            QTextStream stream(&inputFile);

            stream.setCodec("UTF-8");
            while (stream.readLineInto(&line))
            {
                QStringList tokens = line.split("=");

                // dismiss invalid lines
                if (tokens.length() >= 2)
                {
                    // dismiss redundant tokens
                    if (tokens[1].endsWith(QChar{'\n'}))
                        tokens[1] = tokens[1].left(tokens[1].length() - 1);

                    if (charMapDict->contains(tokens[1]))
                    {
                        qDebug() << "duplicate entry: " << line;
                        continue;
                    }
                    
                    charMapDict->insert(tokens[1], tokens[0]);
                    charMapDictRev->insert(tokens[0], tokens[1]);
                }
            }
            this->success = true;
        }
    }
}

CharMapReader::~CharMapReader()
{
    if (charMapDict)
        delete charMapDict;
    if (charMapDictRev)
        delete charMapDictRev;
}

bool CharMapReader::State() const noexcept
{
    return success;
}
