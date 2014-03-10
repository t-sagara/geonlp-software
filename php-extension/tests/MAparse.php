<?php
$msg = '東京大学は文京区の他、目黒区や千葉県柏市にもあり ます。';
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");
$result = $gs->MAparse($msg);
printf("%s\n", $result);
