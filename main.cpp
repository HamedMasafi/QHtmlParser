#include <QCoreApplication>
#include <QString>
#include <cssparser.h>

#include "htmlparser.h"
#include "htmltag.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString html = QString::fromUtf8(R"~(
                   <!DOCTYPE HTML>
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
                    <p id="p1">
                    <b>Sample</b> paragraph متن فارسی
            <img src="image.png" />
                   </p>
                   </body>
                   </html>)~");

    QString css = QString::fromUtf8(R"~(
                                    body{
                                    color: red;
                                    background-color: green;
                                    }
                                    /* comment outside - / */
                                    .p{
                                    background-image: url('image.png');
                                    padding: 2px;
                                    /*commend: none*/
                                    }

                                    )~");

    HtmlParser h;
    h.setHtml(html);
    h.parse();

    HtmlTag *p = h.getElementById("p1");
    qDebug() << "-----------";
    qDebug() << p->outterHtml();
    qDebug() << p->innerText();
    StyleTag *style = dynamic_cast<StyleTag*>(h.getElementsByTagName("style").first());
    auto p_style = style->rules().findBySelector("p");
    qDebug() << "rules for p is" << p_style->rules();
    HtmlTag *htag = h.getElementsByTagName("html").first();
    qDebug() << "CHANGE";
    qDebug() << htag->outterHtml();
    p_style->addRule("color", "red");
    qDebug() << htag->outterHtml();

    CssParser cp;
    cp.parse(css);

    return a.exec();
}
