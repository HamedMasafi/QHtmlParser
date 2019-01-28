#ifndef HTMLTAG_H
#define HTMLTAG_H

#include <QMap>
#include <QString>

class HtmlNode
{
    HtmlNode *_parent;

public:
    HtmlNode();
    virtual ~HtmlNode();
    HtmlNode *parent() const;
    void setParent(HtmlNode *parent);
    virtual QString outterHtml() const
    {
        return  "";
    }
    virtual QString innerText() const
    {
        return  "";
    }

};

class TextNode : public HtmlNode
{

    QString _text;

public:
    TextNode();
    QString text() const;
    void setText(const QString &text);
    QString outterHtml() const;
    QString innerText() const;
};

class HtmlTag : public HtmlNode
{
public:
    QString name;
    QMap<QString, QString> attributes;
    QList<HtmlNode *> childs;
    bool hasCloseTag;

public:
    HtmlTag();

    QString id() const;
    void addAttribute(const QString &name, const QString &value);
    QString outterHtml() const;
    QString innerHtml() const;
    QString innerText() const;
};

#endif // HTMLTAG_H
