<?php
$msg = '東京大学は文京区の他、目黒区や千葉県柏市にもあり ます。';
$gs = new GeonlpService("/usr/local/etc/geonlp.rc");

$result = $gs->MAparseNode($msg);
foreach ($result as $i => $node) {
  printf("%s\t%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
	 $node->surface, $node->partOfSpeech, 
	 $node->subclassification1, $node->subclassification2, $node->subclassification3, 
	 $node->conjugatedForm, $node->conjugationType, 
	 $node->originalForm, $node->yomi, $node->pronunciation);
  if ($node->subclassification2 == "地名語") {
    $geoword_entries = explode("/", $node->subclassification3);
    foreach ($geoword_entries as $geoword_entry) {
      preg_match('/(.+):(.*)/', $geoword_entry, $matches);
      $gw = $gs->MAgetGeowordEntry($matches[1]); // [2] には代表表記が入る
      printf(" | %s\n", json_encode($gw));
    }
  }
}
