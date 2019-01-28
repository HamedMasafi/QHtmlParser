#include "htmltag.h"
#include <QDebug>

HtmlTag::HtmlTag() : HtmlNode ()
{

}

QString HtmlTag::id() const
{
    return attributes.value("id");
}


QString HtmlTag::outterHtml() const
{
    QString html = "<" + name;
    foreach (QString k, attributes.keys())
        html.append(QString(" %1=\"%2\"").arg(k).arg(attributes.value(k)));
    if (hasCloseTag)
        html.append(">");
    else
        html.append(" />");

    html.append(innerHtml());

    if (hasCloseTag)
        html.append("</" + name + ">");
    return html;
}

QString HtmlTag::innerHtml() const
{
    QString html;
    QList<HtmlNode*>::const_iterator i;
    for (i = childs.constBegin(); i != childs.constEnd(); ++i)
        html.append((*i)->outterHtml() + "\n");

    return html;
}

QString HtmlTag::innerText() const
{
    QString html;
    QList<HtmlNode*>::const_iterator i;
    for (i = childs.constBegin(); i != childs.constEnd(); ++i) {
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
