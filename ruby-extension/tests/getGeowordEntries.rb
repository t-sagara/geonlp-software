#!ruby
# -*- coding: utf-8 -*-
require "../GeonlpMA"

gs = GeonlpMA.new("/usr/local/etc/geonlp_ma.rc")

# ケース1: 「東京」を語幹または語全体にもつエントリを列挙する
gs.setCustomThemes('.*'); # 全てのテーマを検索対象
result = gs.getGeowordEntries('東京')
if result.size > 0
  printf("Test 1: Success : '東京' のエントリが %s 件見つかりました。\n", result.size)
else
  printf("Test 1: Fail : '東京' のエントリが見つかりません。\n")
end

# ケース2: 「東京」を検索するが、駅テーマは検索対象としない
gs.setCustomThemes('-.*駅.*');
result2 = gs.getGeowordEntries('東京')
if result2.size < result.size
  printf("Test 2: Success : 駅を除いた '東京' のエントリは %d 件に減少しました。\n", result2.size)
else
  tokyo_stations = []
  result.each do |id, r|
    if /駅/ =~ r['geoword_full']
      tokyo_stations.push(r)
    end
  end
  if tokyo_stations.size == 0
    printf("Test 2: Skip : '東京' には駅のエントリが含まれていません。\n")
  else
    printf("Test 2: Fail : '東京' に含まれる駅のエントリが除去できていません。\n")
  end
end

# ケース3: 架空の地名「アレフガルド」を検索し、エントリがないことを確認する
gs.setCustomThemes('.*');
result = gs.getGeowordEntries('アレフガルド')
if result.size == 0
  printf("Test 3: Success : 架空の地名'アレフガルド' のエントリは見つかりませんでした。\n")
else
  printf("Test 3: Fail : 架空の地名'アレフガルド' のエントリが %s 件見つかりました。\n", result.size)
end
