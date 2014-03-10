#!ruby
# -*- coding: utf-8 -*-
require "../GeonlpMA"

gs = GeonlpMA.new("/usr/local/etc/geonlp_ma.rc")

result = gs.getGeowordEntry(1977) # サンプル geodata の '東京都' ID
if result
  printf("Test 1: Success : ID=1977 のエントリ(%s)が見つかりました。\n", result['geoword'])
else
  printf("Test 1: Fail : ID=1977 のエントリが見つかりません。\n")
end

result = gs.getGeowordEntry(1) # 無効な ID
if result.nil?
  printf("Test 2: Success : ID=1 のエントリは存在していません（falseを返しました）。\n")
else
  printf("Test 2: Fail : ID=1 のエントリ(%s)を返しました。\n", result['geoword'])
end
