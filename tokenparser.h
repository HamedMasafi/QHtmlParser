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
        bool insert_begin_end;
        bool insert_content;

        literal_t(std::wstring b, std::wstring e) {
            begin = b;
            end = e;
            insert_begin_end = false;
        }
        literal_t(std::wstring b, std::wstring e, bool insert) {
            begin = b;
            end = e;
            insert_begin_end = insert;
        }
        literal_t(std::wstring b, std::wstring e, std::wstring i, bool insert) {
            begin = b;
            end = e;
            ignore = i;
            insert_begin_end = insert;
        }
        literal_t(std::wstring b, std::wstring e, std::wstring i, bool insert, bool c) {
            begin = b;
            end = e;
            ignore = i;
            insert_begin_end = insert;
            insert_content = c;
        }
    };

    std::vector<comment_t*> _commants;
    std::vector<literal_t*> _literals;

    std::vector<std::wstring> _char_types;
    std::vector<std::function<int (wint_t)>> _check_fns;

    std::wstring _text;
    std::vector<std::wstring> _tokens;
public:
    token_parser();
    virtual ~token_parser();
    std::vector<std::wstring> parse_tokens();
    virtual void parse();

    std::wstring text() const;
    void setText(const std::wstring &text);

protected:
    std::wstring read_until(const std::wstring &text, size_t &i, std::function<int(int)> fn) const;
    std::wstring read_until(const std::wstring &text, size_t &i, const literal_t *lt) const;
    bool is_valid_token(const std::wstring &token) const;
};

class html_parser : public token_parser
{
public:
    html_parser();
    virtual ~html_parser();
};

class css_parser : public token_parser
{
private:
    static int token(wint_t n);
public:
    css_parser();
    virtual ~css_parser();
};

#endif // TOKENPARSER_H
