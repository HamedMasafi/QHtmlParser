#include "tokenparser.h"

TokenParser::TokenParser()
{

}

QStringList TokenParser::parse(const QString &text) const
{
    QStringList tokensList;
    QString lastToken;
    QChar::Category lastCat = QChar::Mark_SpacingCombining;
    bool isInQuoto = false;
    bool isInTag = false;

    for (int i = 0; i < text.length(); ++i) {
        QChar ch = text.at(i);
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
    return tokensList;
}
