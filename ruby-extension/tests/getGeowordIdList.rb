#!ruby
# -*- coding: utf-8 -*-
require "../GeonlpMA"

gs = GeonlpMA.new("/usr/local/etc/geonlp_ma.rc")

result = gs.getGeowordIdList('東京')
if result.length > 0
  printf("Test 1: Success : '%s'\n", result)
else
  printf("Test 1: Fail : '東京' が見つかりません。\n")
end

result = gs.getGeowordIdList('エラー');
if result.length > 0
  printf("Test 2: Fail : 'エラー' が登録されています。 '%s'\n", result)
else
  printf("Test 2: Success : 'エラー' は見つかりません。\n")
end
