<?php
//$gs = new GeonlpService("/usr/local/etc/geonlp.rc");
$gs = new GeonlpService();

echo "---------- parse() (single) -----------------\n";
$msg = '今日は塩竃市に行きます。';
//$msg = '神保町駅から徒歩３分。';
//$msg = '東京大学は文京区の他、目黒区や千葉県柏市にもあります。';
$request = array("method"=>"parse", "params"=>array($msg), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (array) -----------------\n";
$msg2 = '国立情報学研究所は千代田区にあります。';
$request = array("method"=>"parse", "params"=>array(array($msg, $msg2)), "id"=>2);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (geocoding:true) -----------------\n";
$msg = '国立情報学研究所は神保町駅から徒歩３分、千代田区一ツ橋２－１－１にあります。';
$request = array("method"=>"parse", "params"=>array($msg, array("geocoding"=>true)), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (geocoding:false) -----------------\n";
$msg = '国立情報学研究所は神保町駅から徒歩３分、千代田区一ツ橋２－１－１にあります。';
$request = array("method"=>"parse", "params"=>array($msg, array("geocoding"=>false)), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (remove-class:.*Station.*) -----------------\n";
$msg = '国立情報学研究所は神保町駅から徒歩３分、千代田区一ツ橋２－１－１にあります。';
$request = array("method"=>"parse", "params"=>array($msg, array("remove-class"=>array(".*Station.*"))), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (set-dic:[1,2]) -----------------\n";
$msg = '国立情報学研究所は神保町駅から徒歩３分、千代田区一ツ橋２－１－１にあります。';
$request = array("method"=>"parse", "params"=>array($msg, array("set-dic"=>array(1,2))), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (set-dic:[]) -----------------\n";
$msg = '国立情報学研究所は神保町駅から徒歩３分、千代田区一ツ橋２－１－１にあります。';
$request = array("method"=>"parse", "params"=>array($msg, array("set-dic"=>array())), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- parse() (set-class:[\".*Station.*\"]) -----------------\n";
$msg = '国立情報学研究所は神保町駅から徒歩３分、千代田区一ツ橋２－１－１にあります。';
$request = array("method"=>"parse", "params"=>array($msg, array("set-class"=>array(".*Station.*"))), "id"=>1);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

