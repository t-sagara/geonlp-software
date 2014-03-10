#!ruby
# -*- coding: utf-8 -*-
require "../GeonlpMA"

msg = '東京大学は文京区の他、目黒区や千葉県柏市にもあり ます。'
gs = GeonlpMA.new("/usr/local/etc/geonlp_ma.rc")

result = gs.parse(msg)
printf("%s\n", result)
