#ifndef F_DECODER_H
#define F_DECODER_H

#include <QString>
#include <QHash>
#include "core/read_charmap.h"

class QStrResult : public QString
{
private:
    bool success;
public:
    QStrResult(QString str, bool b = false);
    virtual ~QStrResult();
    bool State() const noexcept;
    void SetState(bool) noexcept;
};

class FDecoder
{
private:
    const CharMapReader *reader;

public:
    FDecoder(CharMapReader *r = nullptr);
    virtual ~FDecoder();
    QStrResult HexToStr(QString &hex) const;
    QStrResult StrToHex(QString &str) const;
};

#endif // F_DECODER_H
