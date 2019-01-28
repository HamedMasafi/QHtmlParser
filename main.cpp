#include <QCoreApplication>

#include "htmlparser.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString html = R"~(
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
                   </body>
                   </html>)~";

    HtmlParser h;
    h.setHtml(html);
    h.parse();
    return a.exec();
}
