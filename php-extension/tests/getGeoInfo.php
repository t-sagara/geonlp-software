<?php
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

$bunkyouku = 'MOcc8Y';
$meguroku = 'ku6BHd';
$kashiwashi = 'fO8TpK';

echo "---------- getGeoInfo() -----------------\n";
$request = array("method"=>"getGeoInfo", "params"=>array(array($bunkyouku, $meguroku, $kashiwashi)), "id"=>4.0);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);
