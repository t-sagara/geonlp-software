<?php
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

echo "---------- parsestructured() -----------------\n";
$msg_struct = json_decode('{"organization": {"surface" : "NII","name" : "国立情報学研究所","tel" : "03-4212-2000（代表）"}}', true);
$msg_rest   = "は千代田区一ツ橋１－２－１にあります。神保町から徒歩3分。";
$request = array("method"=>"parseStructured", "params"=>array(array($msg_struct, $msg_rest)), "id"=>"3");
$response = $gs->proc(json_encode($request));
printf("%s\n", $response);
