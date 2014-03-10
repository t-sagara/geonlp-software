<?php
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

echo "---------- getDictionaryInfo() (single) -----------------\n";
$request = array("method"=>"getDictionaryInfo", "params"=>array(26), "id"=>6);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);

echo "---------- getDictionaryInfo() (array) -----------------\n";
$request = array("method"=>"getDictionaryInfo", "params"=>array(array(27,28)), "id"=>7);
$request_str = json_encode($request);
printf("request:'%s'\n", $request_str);
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);
