#include "htmlparser.h"

#include <QDebug>
#include <QStack>
#include <QStringList>

QString HtmlParser::html() const
{
    return _html;
}

void HtmlParser::setHtml(const QString &html)
{
    _html = html;
}


HtmlParser::HtmlParser()
{

}

void HtmlParser::parse()
{
    QStringList tokensList;
    QString lastToken;
    QChar::Category lastCat = QChar::Mark_SpacingCombining;
    bool isInQuoto = false;
    bool isInTag = false;


    for (int i = 0; i < _html.length(); ++i) {
        QChar ch = _html.at(i);
        if (ch == '\n')
            qDebug() << ch.category();
        if (ch.category() == QChar::Other_Control)
            continue;

        if (ch == "<") {
            if (!isInTag) {
                qDebug() << "TEXT="<<lastToken;
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
        if (ch == "\"") {
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

    QList<Tag*> tags;
    QStack<Tag*> stack;
    _htmlTag = nullptr;
    for (int i = 0; i < tokensList.count(); ++i) {
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
                Tag *tag = parseTagBegin(tokensList, i);
                if (tag) {
                    if (!tags.size())
                        _htmlTag = tag;
                    if (stack.size()) {
                        tag->parent = stack.top();
                        stack.top()->childs.append(tag);
                    }

                    tags.append(tag);
                    if (tag->hasCloseTag)
                        stack.push(tag);

                    if (tag->name == "style")
                        qDebug() << "current token is" << tokensList.at(i + 1);
                }
            }
        }

        token = tokensList.at(i);
        if (token == ">"){
            isInTag = false;
            if (tokensList.size() > i + 1 && tokensList.at(i + 1) != "<")
                stack.top()->text = tokensList.at(i + 1).trimmed();
        }
    }
    printTag(_htmlTag, 0);
}

Tag *HtmlParser::parseTagBegin(QStringList &tokensList, int &i)
{
    Tag *tag = new Tag;
    QString token;
    i++;
    tag->name = tokensList.at(i++);
    QMap<QString, QString> attrs;
    QString name, value;
    int step = 0;
    qDebug() << "searching for" << tag->name;
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
                qDebug() << "error; token is" << token << "in" << step << "step";
                return nullptr;
            }
            break;

        case 2:
            step = 0;
            value = token;
            attrs.insert(name, value);
            break;
        }
    } while (token != ">");

    tag->hasCloseTag = (tokensList.at(i - 2) != "/");
    tag->attributes = attrs;
    i--;
    return tag;
}

void HtmlParser::printTag(Tag *tag, int level)
{
    QString tab;
    for (int i = 0; i < level; ++i)
        tab.append("    ");
    qDebug() << tab + tag->toString();
    if (!tag->text.isEmpty())
        qDebug() << tab + tag->text;

    foreach (Tag *t, tag->childs) {
        printTag(t, level + 1);
    }
}
