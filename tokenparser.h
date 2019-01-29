#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <QStringList>



class TokenParser
{
public:
    TokenParser();
    QStringList parse(const QString &text) const;
};

#endif // TOKENPARSER_H
