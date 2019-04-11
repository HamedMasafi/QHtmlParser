#include "cssrules.h"

#include <QDebug>


QMap<QString, QString> CssNode::rules() const
{
    return _rules;
}

void CssNode::setRules(const QMap<QString, QString> &rules)
{
    _rules = rules;
}

void CssNode::addRule(const QString &name, const QString &value)
{
    _rules.insert(name.toLower(), value);
}

QString CssNode::selector() const
{
    return _selectors.join(", ");
}

bool CssNode::hasSelector(const QString &selector)
{
    return _selectors.contains(selector, Qt::CaseInsensitive);
}

QString CssNode::toString() const
{
    QString ret;
    QMap<QString, QString>::const_iterator i;
    for (i = _rules.constBegin(); i != _rules.constEnd(); ++i) {
        if (!ret.isEmpty())
            ret.append(";");
        ret.append(i.key() + ":" + i.value());
    }
    return selector() + "{" + ret + "}";
}

CssNode::CssNode()
{

}

CssNode::CssNode(const QString &selector)
{
    auto ps = selector.split(",");
    foreach (QString p, ps)
        _selectors.append(p.trimmed());
}

CssNode *CssDoc::findBySelector(const QString &selector)
{
    QList<CssNode*>::iterator i;
    for (i = begin(); i != end(); ++i) {
        if ((*i)->hasSelector(selector))
            return *i;
    }
    return nullptr;
}

QString CssDoc::toString() const
{
    QString ret;
    QList<CssNode*>::const_iterator i;
    for (i = constBegin(); i != constEnd(); ++i) {
        ret.append((*i)->toString() + "\n");
    }
    return ret;
}
