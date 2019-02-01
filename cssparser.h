#ifndef CSSPARSER_H
#define CSSPARSER_H

#include <tokenparser.h>

class css_parser : public token_parser
{
private:
    static int token(wint_t n);
public:
    css_doc doc;
    css_parser();
    void parse();
    virtual ~css_parser();
};

#endif // CSSPARSER_H
