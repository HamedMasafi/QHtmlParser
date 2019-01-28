#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QMap>
#include <QString>
#include <functional>

class HtmlTag;
class HtmlParser
{
    QString _html;
    HtmlTag *_htmlTag;
public:
    HtmlParser();
    void parse();

    QString html() const;
    void setHtml(const QString &html);

    HtmlTag *getElementById(const QString id);
    QList<HtmlTag *> getElementsByTagName(const QString tagName);
    QList<HtmlTag *> getElementsByClassName(const QString className);
private:
    HtmlTag *parseTagBegin(QStringList &tokensList, int &i);
    void printTag(HtmlTag *tag, int level);

    void search(QList<HtmlTag*> *tasg, HtmlTag *tag, int &flag, std::function<bool(const HtmlTag*, int&)> callback);
};

#endif // HTMLPARSER_H
