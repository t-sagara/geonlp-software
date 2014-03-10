<?php
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

echo "---------- getDictionaries() -----------------\n";
$request = array("method"=>"getDictionaries", "params"=>array(), "id"=>"五");
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);
