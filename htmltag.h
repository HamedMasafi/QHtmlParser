#ifndef HTMLTAG_H
#define HTMLTAG_H

#include "cssrules.h"

#include <QMap>
#include <QString>

class CssNode;
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

//class HtmlComment : public TextNode
//{

//};

class HtmlTag : public HtmlNode
{

    QString _name;
    QMap<QString, QString> _attributes;
    QList<HtmlNode *> _childs;
    bool _hasCloseTag;
    CssNode *_css;
    QStringList _classes;
public:
    HtmlTag();
    virtual ~HtmlTag();

    QString id() const;
    QString attribute(const QString &name) const;
    void setAttribute(const QString &name, const QString &value);
    void addClass(const QString &name);
    void removeClass(const QString &name);
    bool hasClass(const QString &name) const;

    virtual void addCHild(HtmlNode *child);

    QString outterHtml() const;
    virtual QString innerHtml() const;
    QString innerText() const;
    QString name() const;
    void setName(const QString &name);
    bool hasCloseTag() const;
    void setHasCloseTag(bool hasCloseTag);
    QList<HtmlNode *> childs() const;
};

class StyleTag : public HtmlTag
{
    CssDoc _rules;

public:
    StyleTag();

    void addCHild(HtmlNode *child);
    CssDoc rules() const;
    void setRules(const CssDoc &rules);

    QString innerHtml() const;
};

#endif // HTMLTAG_H
