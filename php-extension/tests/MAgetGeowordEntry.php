<?php
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

$result = $gs->MAgetGeowordEntry("ku6BHd"); // サンプル geodata の '目黒区' ID
if ($result) {
  printf("Test 1: Success : ID='ku6BHd' のエントリが見つかりました。\n");
  print_r($result);
} else {
  printf("Test 1: Fail : ID='ku6BHd' のエントリが見つかりません。\n");
}

$result = $gs->MAgetGeowordEntry("invalid"); // 無効な ID

if ($result === false) {
  printf("Test 2: Success : ID='invalid' のエントリは存在していません（falseを返しました）。\n");
} else {
  printf("Test 2: Fail : ID='invalid' のエントリを返しました。\n");
  print_r($result);
}

