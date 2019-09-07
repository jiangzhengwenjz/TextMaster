#include "core/f_decoder.h"

QStrResult::QStrResult(QString str, bool b) :
    QString(str),
    success(b)
{
}

QStrResult::~QStrResult()
{
}

bool QStrResult::State() const noexcept
{
    return success;
}

void QStrResult::SetState(bool b) noexcept
{
    success = b;
}

FDecoder::FDecoder(CharMapReader *r) :
    reader(r)
{
}

FDecoder::~FDecoder()
{
}

QStrResult FDecoder::HexToStr(QString &hex) const
{
    QStrResult res{""};

    if (!hex.size())
        return QStrResult{"没有任何输入"};
    for (int i = 0; i < hex.size(); ++i)
    {
        if (hex[i] != QChar(' ') && hex[i] != QChar('\t') && hex[i] != ('\n'))
        {
            if (i < hex.size() - 1 && hex[i].isLetterOrNumber() && hex[i + 1].isLetterOrNumber())
            {
                if (reader->charMapDictRev->contains(hex.mid(i, 2).toUpper()))
                {
                    res.append((*reader->charMapDictRev)[hex.mid(i++, 2).toUpper()]);
                    continue;
                }
                int j;
                for (j = i + 2; j < hex.size(); ++j) // actually a threshold can be assigned here for typical charmaps
                {
                    if (j < hex.size() - 1
                     && hex[j].isLetterOrNumber()
                     && hex[j + 1].isLetterOrNumber()
                     && reader->charMapDictRev->contains(hex.mid(i, j - i + 2).toUpper()))
                    {
                        res.append((*reader->charMapDictRev)[hex.mid(i, j - i + 2).toUpper()]);
                        break;
                    }
                }
                if (j == hex.size())
                    return QStrResult{QString("Cannot decode %1").arg(hex.mid(i, 2)), false};
                i += j - i + 1;
            }
            else
            {
                return QStrResult{QString("Cannot decode %1").arg(hex.mid(i, i < hex.size() - 1 ? 2 : 1)), false};
            }
        }
    }
    res.SetState(true);
    return res;
}

QStrResult FDecoder::StrToHex(QString &str) const
{
    QStrResult res{""};
    int idx;
    
    if (!str.size())
        return QStrResult{"没有任何输入"};

    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == QChar('\n')) // new line
        {
            if (reader->charMapDict->contains(QString("\\n")))
                res.append((*reader->charMapDict)[QString("\\n")]); // otherwise just omit it
        }
        else if (str[i] == QChar('\\')) // escape
        {
            if (i == str.size() - 1) // i is already the last char
            {
                // evaluate this as a fail
                return QStrResult{"Cannot decode \\", false};
            }
            else if (str[i + 1] == QChar('h')) // hex mode
            {
                if (i > str.size() - 4 || !str[i + 2].isLetterOrNumber() || !str[i + 3].isLetterOrNumber())
                {
                    // evaluate this as a fail
                    return QStrResult{QString("Cannot decode \\h%1").arg(str.mid(i + 2)), false};
                }
                else
                {
                    // try to convert the next 2 chars to integer
                    bool ok;
                    str.mid(i + 2, 2).toUInt(&ok, 16);
                    if (!ok)
                    {
                        return QStrResult{QString("Cannot decode \\h%1").arg(str.mid(i + 2, 2)), false};
                    }
                    else
                    {
                        res.append(str.mid(i + 2, 2).toUpper());
                        i += 3;
                    }
                }
            }
            else if (reader->charMapDict->contains(str.mid(i, 2)))
            {
                res.append((*reader->charMapDict)[str.mid(i++, 2)]);
            }
            else
            {
                return QStrResult{QString("Cannot decode \\%1").arg(str.mid(i + 1, 1)), false};
            }

        }
        else if (str[i] == QChar('[') && (idx = str.indexOf(QChar(']'), i + 1)) != -1 && reader->charMapDict->contains(str.mid(i, idx - i + 1)))
        {
            res.append((*reader->charMapDict)[str.mid(i, idx - i + 1)]);
            i += idx - i;
        }
        else // normal mode
        {
            if (reader->charMapDict->contains(str.mid(i, 1)))
                res.append((*reader->charMapDict)[str.mid(i, 1)]);
            else
                return QStrResult{QString("Cannot decode %1").arg(str.mid(i, 1)), false};
        }
    }
    res.SetState(true);
    return res;
}
