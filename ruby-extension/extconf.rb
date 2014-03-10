require "mkmf"
have_library("stdc++")
have_library("sqlite3")
have_library("mecab")
have_library("geonlp_ma")

$CFLAGS = "-I../include"

create_makefile("GeonlpMA")
