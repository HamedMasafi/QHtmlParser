cmake_minimum_required(VERSION 2.6)
project(html_parser)

add_executable(html_parser main.cpp htmlparser.cpp htmlparser.h cssparser.cpp cssparser.h tokenparser.cpp tokenparser.h htmltag.cpp htmltag.h cssrules.h cssrules.cpp string_helper.cpp string_helper.h)

install(TARGETS html_parser RUNTIME DESTINATION bin)
