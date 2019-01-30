#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <functional>
#include <vector>

class token_parser
{
protected:
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

    std::vector<comment_t*> _commants;
    std::vector<literal_t*> _literals;

    std::vector<std::wstring> _char_types;
    std::vector<std::function<int (int)>> _check_fns;

public:
    token_parser();
    std::vector<std::wstring> parse(const std::wstring &text) const;

protected:
    std::wstring read_until(const std::wstring &text, size_t &i, std::function<int(int)> fn) const;
    std::wstring read_until(const std::wstring &text, size_t &i, const std::wstring &end) const;
    bool is_valid_token(const std::wstring &token) const;
};

class html_parser : public token_parser
{
public:
    html_parser();
};

class css_parser : public token_parser
{
public:
    css_parser();
    std::vector<std::wstring> parse(const std::wstring &text) const;
};

#endif // TOKENPARSER_H
