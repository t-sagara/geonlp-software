<?php
$gs = new GeonlpService();

// ケース1: 「塩竃」を語幹または語全体にもつエントリを列挙する
$result = $gs->MAgetGeowordEntries('塩竃');

if (count($result) > 0) {
  printf("Test 1: Success : '静岡' のエントリが %s 件見つかりました。\n", count($result));
} else {
  printf("Test 1: Fail : '静岡' のエントリが見つかりません。\n");
}

// ケース2: 「東京」を検索するが、駅テーマは検索対象としない
$gs->MAsetActiveClasses(array('State.*', 'City.*'));
$result2 = $gs->MAgetGeowordEntries('東京');

if (count($result2) < count($result)) {
  printf("Test 2: Success : 駅を除いた '東京' のエントリは %s 件に減少しました。\n", count($result2));
} else {
  $tokyo_stations = array();
  foreach($result as $i => $r) {
    if (preg_match('/駅/', $r->geoword_full)) {
      $tokyo_stations []= $r;
    }
  }
  if (count($tokyo_stations) == 0) {
    printf("Test 2: Skip : '東京' には駅のエントリが含まれていません。\n");
  } else {
    printf("Test 2: Fail : '東京' に含まれる駅のエントリが除去できていません。\n");
    foreach($tokyo_stations as $i => $s) {
      printf("%s(%s) ", $s->geoword_full, implode(':', $s->upper_geowords));
    }
    printf("\n");
  }
}

// ケース3: 架空の地名「アレフガルド」を検索し、エントリがないことを確認する
$result = $gs->MAgetGeowordEntries('アレフガルド');
if (count($result) == 0) {
  printf("Test 3: Success : 架空の地名'アレフガルド' のエントリは見つかりませんでした。\n");
} else {
  printf("Test 3: Fail : 架空の地名'アレフガルド' のエントリが %s 件見つかりました。\n", count($result));
}

// ケース4: 「とうきょうと」という読みから検索する
$gs->MAresetActiveClasses();
$result = $gs->MAgetGeowordEntries('トウキョウト');
if (count($result) > 0) {
  printf("Test 1: Success : 'トウキョウト' のエントリが %s 件見つかりました。\n", count($result));
} else {
  printf("Test 1: Fail : 'トウキョウト' のエントリが見つかりません。\n");
}
