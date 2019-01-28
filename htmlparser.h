#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QMap>
#include <QString>

class Tag{
public:
    QString name;
    QMap<QString, QString> attributes;
    QList<Tag*> childs;
    QString text;
    bool hasCloseTag;
    Tag *parent;

    QString toString() const
    {
        QString ret = name;
        foreach (QString k, attributes.keys())
            ret.append(QString(" %1=\"%2\"").arg(k).arg(attributes.value(k)));
        return ret;
    }
};

class HtmlParser
{
    QString _html;
    Tag *_htmlTag;
public:
    HtmlParser();
    void parse();

    QString html() const;
    void setHtml(const QString &html);

private:
    Tag *parseTagBegin(QStringList &tokensList, int &i);
    void printTag(Tag *tag, int level);
};

#endif // HTMLPARSER_H
