#include "htmlparser.h"
#include "htmltag.h"

#include <QDebug>
#include <QStack>
#include <QStringList>

HtmlParser::HtmlParser()
{

}

HtmlParser::~HtmlParser()
{
    delete _htmlTag;
}

QString HtmlParser::html() const
{
    return _html;
}

void HtmlParser::setHtml(const QString &html)
{
    _html = html;
}

HtmlTag *HtmlParser::getElementById(const QString id)
{
    QList<HtmlTag*> ret;
    int f = 0;
    search(&ret, _htmlTag, f, [=](const HtmlTag *t, int &) {
       return t->id() == id;
    });
    return ret.size() ? ret.first() : nullptr;
}

QList<HtmlTag *> HtmlParser::getElementsByTagName(const QString tagName)
{
    QList<HtmlTag*> ret;
    int f = 0;
    search(&ret, _htmlTag, f, [=](const HtmlTag *t, int &) {
        qDebug() << "checked" << t->name();
       return t->name().compare(tagName, Qt::CaseInsensitive) == 0;
    });
    return ret;
}

QList<HtmlTag *> HtmlParser::getElementsByClassName(const QString className)
{
    QList<HtmlTag*> ret;
    int f = 0;
    search(&ret, _htmlTag, f, [=](const HtmlTag *t, int &) {
       return t->hasClass(className);
    });
    return ret;
}


void HtmlParser::parse()
{
    //TODO: fix this trick
    _html = _html.replace("'rtl'", "\"rtl\"");
    QStringList tokensList;
    QString lastToken;
    QChar::Category lastCat = QChar::Mark_SpacingCombining;
    bool isInQuoto = false;
    bool isInTag = false;

    for (int i = 0; i < _html.length(); ++i) {
        QChar ch = _html.at(i);
        if (ch.category() == QChar::Other_Control)
            continue;

        if (ch == "<") {
            if (!isInTag) {
                if (!lastToken.trimmed().isEmpty())
                    tokensList.append(lastToken);
            }
            isInTag = true;
            lastCat = ch.category();
            lastToken.clear();
            tokensList.append(ch);
            continue;
        }
        if (!isInTag) {
            lastToken.append(ch);
            lastCat = ch.category();
            continue;
        }
        if (ch == ">") {
            isInTag = false;
            lastCat = ch.category();
            if (!lastToken.isEmpty())
                tokensList.append(lastToken);
            lastToken.clear();
            tokensList.append(ch);
            continue;
        }
        if (ch == "\""/* || (ch == "'" && !isInQuoto)*/) {
            if (!lastToken.isEmpty())
                tokensList.append(lastToken);
            lastToken.clear();
            isInQuoto = !isInQuoto;
            lastCat = ch.category();

            continue;
        }
        if (isInQuoto) {
            lastToken.append(ch);
            continue;
        }

        if (ch.isSpace()) {
            if (!lastToken.isEmpty())
                tokensList.append(lastToken);
            lastCat = ch.category();
            lastToken.clear();
            continue;
        }

        if (lastCat != ch.category()) {
            if (!lastToken.isEmpty())
                tokensList.append(lastToken);
            lastToken = ch;
            lastCat = ch.category();
        } else {
            lastToken.append(ch);
        }
    }
    qDebug() << _html;
    qDebug() << "-----------";
    qDebug() << tokensList;//.join("\n");

    QList<HtmlTag*> tags;
    QStack<HtmlTag*> stack;
    _htmlTag = nullptr;
    QString doctype;
    int i = 0;

    if (tokensList.count() > 3 && tokensList.at(0) == "<" && tokensList.at(1) == "!") {
        QString token;
        while (token != ">") {
            token = tokensList.at(++i);
            if (token != ">")
                doctype.append(token + " ");
        }
    }
    qDebug() << "doc type is" << doctype;
    for (; i < tokensList.count(); ++i) {
        QString token = tokensList.at(i);

        if (token == "<") {
            isInTag = true;
            if (tokensList.count() <= i + 1) {
                qDebug() << "Unexpected end of document";
                return;
            }

            if (tokensList.at(i + 1) == "/") {
                stack.pop();
            } else {
                HtmlTag *tag = parseTagBegin(tokensList, i);
                if (tag) {
                    if (!tags.size())
                        _htmlTag = tag;
                    if (stack.size()) {
                        tag->setParent(stack.top());
                        stack.top()->addCHild(tag);
                    }

                    tags.append(tag);
                    if (tag->hasCloseTag())
                        stack.push(tag);
                }
            }
        }

        token = tokensList.at(i);
        if (token == ">"){
            isInTag = false;
            if (tokensList.size() > i + 1 && tokensList.at(i + 1) != "<") {
                TextNode *textNode = new TextNode;
                textNode->setText(tokensList.at(i + 1).trimmed());
                stack.top()->addCHild(textNode);
            }
        }
    }
    printTag(_htmlTag, 0);
    qDebug() << _htmlTag->outterHtml();
}

HtmlTag *HtmlParser::parseTagBegin(QStringList &tokensList, int &i)
{
    HtmlTag *tag;
    QString token;
    i++;
    auto tagName = tokensList.at(i++);
    if (tagName.compare("style", Qt::CaseInsensitive) == 0)
        tag = new StyleTag;
    else
        tag = new HtmlTag;

    tag->setName(tagName);
    QMap<QString, QString> attrs;
    QString name, value;
    int step = 0;
    /*

    name
     =
     "
     value
     "

     */
    if (tokensList.at(i) != ">")
        do {
        if (i == tokensList.count()) {
            qDebug() << "Unexpected end of document";
            delete tag;
            return nullptr;
        }
        token = tokensList.at(i++);

        if (token == ">")
            break;

        switch (step) {
        case 0:
            name = token;
            step++;
            break;

        case 1:
            if (token == "=")
                step++;
            else {
                qDebug() << "error; token is" << token << "in" << step << "step for" << tag->name();
                delete tag;
                return nullptr;
            }
            break;

        case 2:
            step = 0;
            value = token;
            tag->setAttribute(name, value);
            break;
        }
    } while (token != ">");

    tag->setHasCloseTag(tokensList.at(i - 2) != "/");
    i--;
    return tag;
}

void HtmlParser::printTag(HtmlTag *tag, int level)
{
//    QString tab;
//    for (int i = 0; i < level; ++i)
//        tab.append("    ");
//    qDebug() << tab + tag->toString();

//    foreach (HtmlTag *t, tag->childs) {
//        printTag(t, level + 1);
//    }
}

void HtmlParser::search(QList<HtmlTag *> *tags, HtmlTag *tag, int &flag,
                        std::function<bool (const HtmlTag *, int &)> callback)
{
    if (callback(tag, flag))
        tags->append(tag);
    foreach (HtmlNode *node, tag->childs()) {
        HtmlTag *t = dynamic_cast<HtmlTag*>(node);
        if (t)
            search(tags, t, flag, callback);
    }
}
