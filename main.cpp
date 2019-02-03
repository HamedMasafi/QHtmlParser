#include <css_parser.h>
#include <token_parser.h>
#include <iostream>
#include <algorithm>

#include "html_parser.h"
#include "html_node.h"
#include "query_parser.h"

int main(int argc, char *argv[])
{

    auto html = LR"~(<!DOCTYPE HTML>
<html>
<head>
<meta name="qrichtext" content="1" />
<style type="text/css">
p, li { white-space: pre-wrap; }
b { color: "red" }
</style>
</head>
<body style=" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;">
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">dfg</p>
<img src="image.png" />
<p id="p1" class="par">
<b>Sample</b> paragraph متن فارسی
<img src="image.png" />
</p>
</body>
</html>)~";

    auto css = LR"~(
body{
color: red;
background-color: green;
}
body div{
    display: block;
}
/*comment outside - /*/
.p,div{
background-image: url('image.png');
padding: 2px;
                                    border: 1px solid green;
/*comment: none*/color:red;
}

)~";

    auto print_list = [](std::vector<std::wstring> tokens){
        std::cout << "==== TOKENS ====" << std::endl;
        for (std::wstring t : tokens) {
            std::wcout << L"\"" << t << L"\" ";
        }
        std::wcout << "================" << std::endl;
    };
    auto print_string = [](std::string title, std::wstring text){
        std::cout << "==== " + title + " ====" << std::endl;
        std::wcout << text << std::endl;
        std::cout << "================" << std::endl;
    };

    html_parser tp;
    tp.set_text(html);

    auto tags = tp.query(L"p.par tag b, html body img, html head meta");
    std::for_each(tags.begin(), tags.end(), [](html_tag *tag){
        tag->set_attr(L"data-changed", L"true");
    });

    css_parser cp;
    cp.set_text(css);

    print_list(tp.tokens());
    print_list(cp.tokens());
    print_string("HTML compact", tp.to_string());
    print_string("HTML formatted", tp.to_string(print_type::formatted));
    print_string("CSS compact", cp.doc.to_string());
    print_string("CSS formatted", cp.doc.to_string(print_type::formatted));

    std::wcout << "END" << std::endl;
    return EXIT_SUCCESS;
}
