<?php
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

echo "---------- addressGeocoding() (single) -----------------\n";
$msg = '千代田区一ツ橋２－１－２';
$opts = array("geocoding"=>"full");
$request = array("method"=>"addressGeocoding", "params"=>array($msg, $opts), "id"=>8);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- addressGeocoding() (array) -----------------\n";
$msg2 = '東京都多摩市永山一丁目５';
$request = array("method"=>"addressGeocoding", "params"=>array(array($msg, $msg2)), "id"=>2);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);
