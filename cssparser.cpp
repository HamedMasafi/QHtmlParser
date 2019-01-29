#include "cssparser.h"
#include "cssrules.h"
#include <QDebug>

CssDoc CssParser::rules() const
{
    return _rules;
}

void CssParser::setRules(const CssDoc &rules)
{
    _rules = rules;
}

CssParser::CssParser()
{

}

void CssParser::parse(const QString &css)
{
    foreach (CssNode *r, _rules)
        delete r;
    _rules.clear();

    QString cssDoc = css + " ";
    bool insideScope = false;
    bool insideComment = false;
    QString lastBlock;

    for (int i = 0; i < cssDoc.length(); ++i) {
        auto ch = cssDoc.at(i);

        if (ch.category() == QChar::Other_Control)
            continue;

        if (ch == "*" && cssDoc.at(i + 1) == "/") {
            insideComment = false;
            i++;
            continue;
        }
        if (ch == "/" && cssDoc.at(i + 1) == "*")
            insideComment = true;

        if (insideComment)
            continue;


        if (ch == "{") {
            insideScope = true;
            CssNode *r = new CssNode(lastBlock.trimmed());
            _rules.append(r);
            lastBlock.clear();
            continue;
        }

        if (ch == "}") {
            insideScope = false;;
            _rules.last()->setRules(parseRules(lastBlock.trimmed()));
            lastBlock.clear();
            continue;
        }
        lastBlock.append(ch);
    }

    foreach (CssNode *r, _rules) {
        qDebug() << r->toString();
    }
}

QMap<QString, QString> CssParser::parseRules(const QString &rules) const
{
    QMap<QString, QString> ret;
    auto parts = rules.split(";");
    foreach (QString p, parts) {
        auto ps = p.split(":");
        if (ps.size() != 2)
            continue;
        ret.insert(ps.at(0).trimmed(), ps.at(1).trimmed());
    }
    return ret;
}
