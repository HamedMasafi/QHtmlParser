#include <QDebug>
#include "cssrules.h"
#include "tokenparser.h"
#include <cctype>
#include <iostream>

using namespace std;

token_parser::token_parser()
{
}

std::vector<std::wstring> token_parser::parse(const std::wstring &text) const
{
    std::vector<std::wstring> tokens;
    std::wstring ignores = L" \t\r\n";
    int last_cat = -1;

    for (std::size_t i = 0; i < text.length(); ++i) {
        std::wstring last_token;
        auto ch = text.at(i);
//        uchar cat = get_char_cat(ch);
        //wcout << "char is" << text.substr(i, 1) << endl;
        if (iswcntrl(ch) || iswspace(ch) || iswblank(ch) || ch == '\n' || ch == '\r')
        {
            qDebug() << text.substr(i, 1) << "is space";
            continue;
        }
        if (i == 52)
            qDebug() << 52;

        bool outer_continue = false;
        for (literal_t *literal : _literals) {
            wstring st = text.substr(i, literal->begin.length());
            if (st == literal->begin) {
                last_token = read_until(text, i, literal->end);

                if (literal->insert_into_tokens)
                    tokens.push_back(literal->begin);
                if (is_valid_token(last_token))
                    tokens.push_back(last_token);

                if (literal->insert_into_tokens)
                    tokens.push_back(literal->end);

                outer_continue = true;
                break;
            }
        }
        if (outer_continue)
            continue;

        for (auto &fn : _check_fns)
            if (fn(ch)) {
                last_token = read_until(text, i, fn);
                if (last_token.length()) {
                    tokens.push_back(last_token);
                    --i;
                    continue;
                }
            }

        qDebug() << "missed" << text.substr(i, 1);
        if (iswpunct(ch)) {
            tokens.push_back(text.substr(i, 1));
            qDebug() << text.at(i) << ch << text.substr(i, 1);
        }
//        last_cat = get_char_cat(ch);
    }

    QList<QString> tk;
    for (wstring t : tokens) {
        tk.append(QString::fromStdWString(t));
    }
    //    wcout << L"RESULT======================" <<endl;
    //    wcout << text << endl;
    //    wcout << L"TOKENS===========" << endl;
    //    for (wstring t : tokens)
    //        wcout << L"\"" << t << L"\" ";
    ////        wcout << t;
    ////    wcout << endl << flush;
    //    wcout << "End" << endl;
    qDebug() << QString::fromStdWString(text);
    qDebug() << tk;
    return tokens;
}

wstring token_parser::read_until(const wstring &text, size_t &i, std::function<int (int)> fn) const
{
    size_t start = i;
    while (fn(text.at(i))) {
        i++;
    }
    return text.substr(start, i - start);
}

wstring token_parser::read_until(const wstring &text, size_t &i, const wstring &end) const
{
    auto start = ++i;
    while (i < text.length() - end.length()) {
        if (text.length() < i + end.length())
            return wstring();

        auto s = text.substr(i, end.length());
        if (s == end)
            break;
        ++i;
    }
    if (i == start)
        return wstring();
//    qDebug() << "Read" << end << text.substr(start, i - start);

    return text.substr(start, i - start);
}

bool token_parser::is_valid_token(const wstring &token) const
{
    if (!token.length())
        return false;

    return any_of(token.begin(), token.end(), [](wchar_t ch){
        return iswprint(ch);
    });
}

html_parser::html_parser() : token_parser()
{
    _literals.push_back(new literal_t(L">", L"<", true));
    _literals.push_back(new literal_t(L"\"", L"\"", L"\\\"", false));
    _literals.push_back(new literal_t(L"'", L"'", L"\\'", false));

    _check_fns.push_back(&iswalpha);
    _check_fns.push_back(&iswdigit);
}

css_parser::css_parser()
{
    _literals.push_back(new literal_t(L"'", L"'", L"\\'", false));
    _literals.push_back(new literal_t(L"/*", L"*/", true));

    _check_fns.push_back(&iswalpha);
    _check_fns.push_back(&iswdigit);

    _commants.push_back(new comment_t{L"/*", L"*/"});
}

static inline void ltrim(std::wstring &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wchar_t ch) {
        return !iswspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::wstring &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](wchar_t ch) {
        return !iswspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::wstring &s) {
    ltrim(s);
    rtrim(s);
}

std::vector<wstring> css_parser::parse(const wstring &text) const
{
    size_t i = 0;
    while (true) {
        wstring selector = read_until(text, i, L"{");
        wstring inside = read_until(text, i, L"}");
        size_t in = 0;
        while (true) {
            wstring name = read_until(inside, in, L":");
            wstring value = read_until(inside, in, L";");
            trim(name);
            trim(value);
            qDebug() << name <<"="<< value;
            if (in >= inside.length())
                break;
        }
        qDebug() << selector << inside;
        if (i >= text.length())
            break;
    }
    return vector<wstring>();

    wstring css_doc = text + L" ";
    bool inside_scope = false;
    bool inside_comment = false;
    wstring last_block;

    for (size_t i = 0; i < css_doc.length(); ++i) {
        auto ch = css_doc.at(i);

        if (!iswprint(ch))
            continue;

        if (ch == '*' && css_doc.at(i + 1) == '/') {
            inside_comment = false;
            i++;
            continue;
        }
        if (ch == '/' && css_doc.at(i + 1) == '*')
            inside_comment = true;

        if (inside_comment)
            continue;


        if (ch == '{') {
            inside_scope = true;
            trim(last_block);
            CssNode *r = new CssNode(QString::fromStdWString(last_block));
//            _rules.append(r);
            qDebug() << "block is" << last_block;
            last_block.clear();
            continue;
        }

        if (ch == '}') {
            inside_scope = false;;
            trim(last_block);
//            _rules.last()->setRules(parseRules(lastBlock));
            last_block.clear();
            continue;
        }
        last_block.append(text.substr(i, 1));
    }


    return vector<wstring>();
}
