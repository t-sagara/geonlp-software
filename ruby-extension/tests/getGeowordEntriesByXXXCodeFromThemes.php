<?php
$gs = new GeonlpMA("/usr/local/etc/geonlp_ma.rc");

// ケース1: 都道府県コード「13」を持つエントリを '都道府県' テーマから検索
$result = $gs->getGeowordEntriesByStateCodeFromThemes('13', '都道府県|JISX0401');
if (count($result) == 1) {
  $res = array_pop($result);
  if ($res->geoword_full == '東京都') {
    printf("Test 1: Success : 都道府県コード 13 を持つエントリ '東京都' が見つかりました。\n");
  } else {
    printf("Test 1: Fail : 都道府県コード 13 を持つエントリが '東京都' ではありませんでした。\n");
    print_r($res);
  }
} else if (count($result) == 0) {
  printf("Test 1: Fail : 都道府県コード 13 を持つエントリが見つかりませんでした。\n");
} else {
  printf("Test 1: Fail : 都道府県コード 13 を持つエントリが複数見つかりました。\n");
  print_r($result);
}

// ケース2: 市区町村コード「13101」を持つエントリを '市区町村' テーマから検索
$result = $gs->getGeowordEntriesByCityCodeFromThemes('13101', '市.*町村|JISX0402');
if (count($result) == 1) {
  $res = array_pop($result);
  if ($res->geoword_full == '千代田区') {
    printf("Test 2: Success : 市区町村コード 13101 を持つエントリ '千代田区' が見つかりました。\n");
  } else {
    printf("Test 2: Fail : 市区町村コード 13101 を持つエントリが '千代田区' ではありませんでした。\n");
    print_r($res);
  }
} else if (count($result) == 0) {
  printf("Test 2: Fail : 市区町村コード 13101 を持つエントリが見つかりませんでした。\n");
} else {
  printf("Test 2: Fail : 市区町村コード 13101 を持つエントリが複数見つかりました。\n");
  print_r($result);
}

