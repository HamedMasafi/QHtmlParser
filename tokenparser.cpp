#include <QDebug>
#include "cssrules.h"
#include "tokenparser.h"
#include <cctype>
#include <iostream>

using namespace std;

QDebug operator<< (QDebug d, const std::wstring &s) {
    d << QString::fromStdWString(s);
    return d;
}

std::wstring token_parser::text() const
{
    return _text;
}

void token_parser::setText(const std::wstring &text)
{
    _text = text;
}

token_parser::token_parser()
{
}

token_parser::~token_parser()
{

}

std::vector<std::wstring> token_parser::parse_tokens()
{
    std::wstring ignores = L" \t\r\n";

    for (std::size_t i = 0; i < _text.length(); ++i) {
        std::wstring last_token;
        auto ch = static_cast<wint_t>(_text.at(i));
//        uchar cat = get_char_cat(ch);
        //wcout << "char is" << text.substr(i, 1) << endl;
        if (iswcntrl(ch) || iswspace(ch) || iswblank(ch) || ch == '\n' || ch == '\r')
            continue;

        bool outer_continue = false;
        for (literal_t *literal : _literals) {
            wstring st = _text.substr(i, literal->begin.length());
            if (st == literal->begin) {
                last_token = read_until(_text, i, literal);

                if (literal->insert_content) {
                    if (literal->insert_begin_end)
                        _tokens.push_back(literal->begin);
                    if (is_valid_token(last_token))
                        _tokens.push_back(last_token);

                    if (literal->insert_begin_end)
                        _tokens.push_back(literal->end);
                }

                outer_continue = true;
                break;
            }
        }
        if (outer_continue)
            continue;

        for (auto &fn : _check_fns)
            if (fn(ch)) {
                last_token = read_until(_text, i, fn);
                if (last_token.length()) {
                    _tokens.push_back(last_token);
                    --i;
                    continue;
                }
            }

        if (iswpunct(ch)) {
            _tokens.push_back(_text.substr(i, 1));
        }
//        last_cat = get_char_cat(ch);
    }

    QList<QString> tk;
    for (wstring t : _tokens) {
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

    return _tokens;
}

void token_parser::parse()
{
    parse_tokens();
}

wstring token_parser::read_until(const wstring &text, size_t &i, std::function<int (int)> fn) const
{
    size_t start = i;
    while (fn(text.at(i))) {
        i++;
    }
    return text.substr(start, i - start);
}

wstring token_parser::read_until(const wstring &text, size_t &i, const literal_t *lt) const
{
    auto start = i;
    while (i < text.length() - lt->end.length()) {
        if (text.length() < i + lt->end.length())
            return wstring();

        auto s = text.substr(i, lt->end.length());
        if (s == lt->end)
            break;
        ++i;
    }
    if (i == start)
        return wstring();
    i += lt->end.length() - 1;
    start += lt->begin.length();

    return text.substr(start, i - start - lt->end.length());
}

bool token_parser::is_valid_token(const wstring &token) const
{
    if (!token.length())
        return false;

    return any_of(token.begin(), token.end(), [](wint_t ch){
        return iswprint(ch);
    });
}

html_parser::html_parser() : token_parser()
{
    _literals.push_back(new literal_t(L">", L"<", L"", true, true));
    _literals.push_back(new literal_t(L"\"", L"\"", L"\\\"", false));
    _literals.push_back(new literal_t(L"'", L"'", L"\\'", false));

    _check_fns.push_back(&iswalpha);
    _check_fns.push_back(&iswdigit);
}

html_parser::~html_parser()
{

}

int css_parser::token(wint_t n)
{
    return iswprint(n) && n != '{' && n != '}' && n != ':' && n != ';' && n != ',';
}

css_parser::css_parser()
{
    _literals.push_back(new literal_t(L"'", L"'", L"\\'", false));
    _literals.push_back(new literal_t{L"/*", L"*/", L"", false, false});

//    _check_fns.push_back(&iswalpha);
//    _check_fns.push_back(&iswdigit);
    _check_fns.push_back(&css_parser::token);

    _commants.push_back(new comment_t{L"/*", L"*/"});
}

css_parser::~css_parser()
{

}

static inline void ltrim(std::wstring &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wint_t ch) {
        return !iswspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::wstring &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](wint_t ch) {
        return !iswspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::wstring &s) {
    ltrim(s);
    rtrim(s);
}

