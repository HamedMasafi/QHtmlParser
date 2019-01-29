#ifndef CSSPARSER_H
#define CSSPARSER_H

#include "cssrules.h"
#include <QString>

class CssNode;
class CssParser
{
    CssDoc _rules;

public:
    CssParser();
    void parse(const QString &css);
    QMap<QString, QString> parseRules(const QString &rules) const;
    CssDoc rules() const;
    void setRules(const CssDoc &rules);
};

#endif // CSSPARSER_H
