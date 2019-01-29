#include "cssparser.h"
#include "cssrules.h"
#include "htmltag.h"
#include <QDebug>

QString HtmlTag::name() const
{
    return _name;
}

void HtmlTag::setName(const QString &name)
{
    _name = name;
}

bool HtmlTag::hasCloseTag() const
{
    return _hasCloseTag;
}

void HtmlTag::setHasCloseTag(bool hasCloseTag)
{
    _hasCloseTag = hasCloseTag;
}

QList<HtmlNode *> HtmlTag::childs() const
{
    return _childs;
}

HtmlTag::HtmlTag() : HtmlNode (), _css(new CssNode)
{

}

HtmlTag::~HtmlTag()
{
    foreach (HtmlNode *child, _childs)
        delete child;
}

QString HtmlTag::id() const
{
    return _attributes.value("id");
}

QString HtmlTag::attribute(const QString &name) const
{
    return _attributes.value(name);
}

void HtmlTag::setAttribute(const QString &name, const QString &value)
{
    auto n = name.toLower();
    if (n == "style") {
        CssParser p;
        _css->setRules(p.parseRules(value));
        qDebug() << "css" << _css->rules();
    } else if (n == "class") {
        _classes = value.split(" ");
    }

    _attributes.insert(n, value);
}

void HtmlTag::addClass(const QString &name)
{
    if (!_classes.contains(name, Qt::CaseInsensitive))
        _classes.append(name);
}

void HtmlTag::removeClass(const QString &name)
{
    _classes.removeOne(name);
}

bool HtmlTag::hasClass(const QString &name) const
{
    return _classes.contains(name, Qt::CaseInsensitive);
}

void HtmlTag::addCHild(HtmlNode *child)
{
    _childs.append(child);
    child->setParent(this);
}


QString HtmlTag::outterHtml() const
{
    QString html = "<" + _name;
    foreach (QString k, _attributes.keys())
        html.append(QString(" %1=\"%2\"").arg(k).arg(_attributes.value(k)));
    if (_hasCloseTag)
        html.append(">");
    else
        html.append(" />");

    html.append(innerHtml());

    if (_hasCloseTag)
        html.append("</" + _name + ">");
    return html;
}

QString HtmlTag::innerHtml() const
{
    QString html;
    QList<HtmlNode*>::const_iterator i;
    for (i = _childs.constBegin(); i != _childs.constEnd(); ++i)
        html.append((*i)->outterHtml() + "\n");

    return html;
}

QString HtmlTag::innerText() const
{
    QString html;
    QList<HtmlNode*>::const_iterator i;
    for (i = _childs.constBegin(); i != _childs.constEnd(); ++i) {
        QString buffer = (*i)->innerText();
        if (!buffer.isEmpty()) {
            if (!buffer.endsWith(" "))
                buffer.append(" ");
            html.append(buffer);
        }
    }

    return html.trimmed();
}

HtmlNode::HtmlNode()
{

}

HtmlNode::~HtmlNode()
{

}

HtmlNode *HtmlNode::parent() const
{
    return _parent;
}

void HtmlNode::setParent(HtmlNode *parent)
{
    _parent = parent;
}

TextNode::TextNode() : HtmlNode ()
{

}

QString TextNode::text() const
{
    return _text;
}

void TextNode::setText(const QString &text)
{
    _text = text;
}

QString TextNode::outterHtml() const
{
    return _text;
}

QString TextNode::innerText() const
{
    return  _text;
}

CssDoc StyleTag::rules() const
{
    return _rules;
}

void StyleTag::setRules(const CssDoc &rules)
{
    _rules = rules;
}

QString StyleTag::innerHtml() const
{
    return _rules.toString();
}

StyleTag::StyleTag() : HtmlTag()
{

}

void StyleTag::addCHild(HtmlNode *child)
{
    TextNode *tn = dynamic_cast<TextNode*>(child);
    if (tn) {
        CssParser cp;
        cp.parse(tn->text());
        _rules = cp.rules();
    } else {
        qDebug() << "Appending non-text node to style tag was not allowed";
    }
}
