#ifndef CSSPARSER_H
#define CSSPARSER_H

#include "css_doc.h"
#include "token_parser.h"

class css_parser : public token_parser
{
private:
    static int token(wint_t n);
public:
    css_doc doc;
    css_parser();
    virtual ~css_parser();
private:
    void parse();
};

#endif // CSSPARSER_H
