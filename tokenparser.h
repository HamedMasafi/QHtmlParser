#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <QStringList>

#include <vector>

class TokenParser
{
    struct comment_t {
        std::wstring begin;
        std::wstring end;
    };

    struct literal_t {
        std::wstring begin;
        std::wstring end;
        std::wstring ignore;
        bool insert_into_tokens;

        literal_t(std::wstring b, std::wstring e) {
            begin = b;
            end = e;
            insert_into_tokens = false;
        }
        literal_t(std::wstring b, std::wstring e, bool insert) {
            begin = b;
            end = e;
            insert_into_tokens = insert;
        }
        literal_t(std::wstring b, std::wstring e, std::wstring i, bool insert) {
            begin = b;
            end = e;
            ignore = i;
            insert_into_tokens = insert;
        }
    };

    std::vector<comment_t> _commants;
    std::vector<literal_t*> _literals;

    std::vector<std::wstring> _char_types;

public:
    TokenParser();
    QStringList parse(const QString &text) const;
    std::vector<std::wstring> parse(const std::wstring &text) const;

private:
    void read_alpha(const std::wstring &text, std::wstring &out, size_t &i) const;
    void read_digit(const std::wstring &text, std::wstring &out, size_t &i) const;
    void read_literal(const std::wstring &text, std::wstring &out, size_t &i, literal_t *literal) const;
    uchar get_char_cat(const wchar_t &wc) const;
};

#endif // TOKENPARSER_H
