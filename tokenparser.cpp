#include <QDebug>
#include "tokenparser.h"
#include <cctype>
#include <iostream>

using namespace std;

TokenParser::TokenParser()
{
    _char_types.push_back(L"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM");
    _char_types.push_back(L"0123456789");

    _literals.push_back(new literal_t(L">", L"<", true));
    _literals.push_back(new literal_t(L"\"", L"\"", L"\\\"", false));
    _literals.push_back(new literal_t(L"'", L"'", L"\\'", false));
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

uchar TokenParser::get_char_cat(const wchar_t &wc) const
{
    if (std::isdigit(wc)) return 1;
    if (std::isalpha(wc)) return 2;
    if (std::isspace(wc)) return 3;
    if (!std::isprint(wc)) return 4;
    if (std::isgraph(wc)) return 5;
    if (std::ispunct(wc)) return 6;
    return 7;
}

std::vector<std::wstring> TokenParser::parse(const std::wstring &text) const
{
    std::vector<std::wstring> tokens;
    std::wstring ignores = L" \t\r\n";
    std::wstring last_token;
    int last_cat = -1;

    for (std::size_t i = 0; i < text.length(); ++i) {
        auto ch = text.at(i);
        uchar cat = get_char_cat(ch);
wcout << "char is" << text.substr(i, 1) << endl;
        if (isspace(ch) || isblank(ch) || ch == '\n' || ch == '\r')
            continue;

        for (literal_t *literal : _literals) {
            wstring st = text.substr(i, literal->begin.length());
            if (st == literal->begin) {
                read_literal(text, last_token, i, literal);

                if (literal->insert_into_tokens)
                    tokens.push_back(literal->begin);
                if (last_token.length()) {
                    tokens.push_back(last_token);
                    i--;
                }
                if (literal->insert_into_tokens)
                    tokens.push_back(literal->end);
                last_token.clear();
                continue;
            }
        }

        if (isalpha(ch)) {
            read_alpha(text, last_token, i);
            if (last_token.length()) {
                tokens.push_back(last_token);
                last_token.clear();
                --i;
                continue;
            }
        }
        if (isdigit(ch)) {
            read_digit(text, last_token, i);
            if (last_token.length()) {
                tokens.push_back(last_token);
                last_token.clear();
                --i;
                continue;
            }
        }

        tokens.push_back(text.substr(i, 1));
//        last_token.append(text.substr(i, 1));
        last_cat = get_char_cat(ch);
    }

    QList<QString> tk;
    for (wstring t : tokens) {
        tk.append(QString::fromStdWString(t));
    }
    wcout << L"RESULT======================" <<endl;
    wcout << text << endl;
    wcout << L"TOKENS===========" << endl;
    for (wstring t : tokens)
        wcout << L"\"" << t << L"\" ";
//        wcout << t;
    wcout << flush;

    return tokens;
}

void TokenParser::read_alpha(const wstring &text, wstring &out, size_t &i) const
{
    size_t start = i;
    while (isalpha(text.at(i))) {
        i++;
    }
    out = text.substr(start, i - start);
    wcout << L"last char is" << text.substr(i, 1) << L"===" << out << endl;
//    if (out.length())
//        --i;
}

void TokenParser::read_digit(const wstring &text, wstring &out, size_t &i) const
{
    size_t start = i;
    while (isdigit(text.at(i))) {
        i++;
    }
    out = text.substr(start, i - start);
}

void TokenParser::read_literal(const wstring &text, wstring &out, size_t &i, TokenParser::literal_t *literal) const
{
    bool end_of_literal = false;
//    i += literal.begin.length();
    size_t start = i;
    while (!end_of_literal) {
        if (i + literal->end.length() > text.length())
            return;

        wstring en = text.substr(i, literal->end.length());
        if (en.compare(literal->end) == 0)
            break;
        i++;
    }
    if (start == i)
        return;

    wcout << L"End of lt:" <<  start << L" - " << i - start - literal->end.length();
    out = text.substr(start + literal->begin.length(), i - start - literal->end.length());
//    i += literal.end.length();
}
