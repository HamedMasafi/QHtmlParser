#ifndef CSSRULES_H
#define CSSRULES_H

#include <QMap>
#include <QString>

class CssNode
{
    QStringList _selectors;
    QMap<QString, QString> _rules;

public:
    CssNode();
    CssNode(const QString &selector);
    QMap<QString, QString> rules() const;
    void setRules(const QMap<QString, QString> &rules);

    void addRule(const QString &name, const QString &value);

    QString selector() const;
    bool hasSelector(const QString &selector);


    QString toString() const;
};

class CssDoc : public QList<CssNode*>
{
public:
    CssNode *findBySelector(const QString &selector);
    QString toString() const;

};

#endif // CSSRULES_H
