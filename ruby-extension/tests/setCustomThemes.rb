#!ruby
# -*- coding: utf-8 -*-
require "../GeonlpMA"

gs = GeonlpMA.new("/usr/local/etc/geonlp_ma.rc")
sample = '東京大学は文京区の他、目黒区や千葉県柏市にもあります。'

def show_reason(gs, result)
  printf("アクティブなテーマID: %s\n", gs.getActiveThemeIds().join(",")) # アクティブなテーマIDリスト
  printf("パース結果 ------\n%s\n", result)                              # パース結果
end  

# ケース1: 「研究機関」テーマを利用しない
gs.setCustomThemes('-研究機関');
result = gs.parse(sample);

if /東京大学\t名詞,固有名詞,組織/m =~ result
  printf("Test 1: Success : 「東京大学」は地名として抽出されませんでした。\n")
else
  printf("Test 1: Fail : 「東京大学」が地名として抽出されました。\n")
  show_reason(gs, result)
end

# ケース2: 「研究機関」テーマを利用する
gs.setCustomThemes('研究機関')
result = gs.parse(sample)

if /東京大学\t名詞,固有名詞,地名語/m =~ result
  printf("Test 2: Success : 「東京大学」が地名として抽出されました。\n")
else
  printf("Test 2: Fail : 「東京大学」は地名として抽出されませんでした。\n")
  show_reason(gs, result)
end

sample = '神田の次は東京です。'

# ケース1: 「空港」を含むテーマを利用しない
gs.setCustomThemes('-.*空港.*')
result = gs.parse(sample)

if /東京\t.*国際空港/m !~ result
  printf("Test 1: Success : 「東京」は「空港」として抽出されませんでした。\n")
else
  printf("Test 1: Fail : 「東京」は「空港」として抽出されました。\n")
  show_reason(gs, result)
end

# ケース2: 「空港」テーマを利用する
gs.setCustomThemes('空港')
result = gs.parse(sample)

if /東京\t.*国際空港/m =~ result
  printf("Test 1: Success : 「東京」は「空港」として抽出されました。\n")
else
  printf("Test 1: Fail : 「東京」は「空港」として抽出されませんでした。\n")
  show_reason(gs, result)
end

# リセットのテスト
gs.resetCustomThemes();
printf("Test 3: Success : resetCustomThemes() が正常に実行されました。\n")
