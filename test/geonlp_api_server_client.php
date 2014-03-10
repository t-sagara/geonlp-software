<?php
error_reporting(E_ALL);

echo ">>> Preparing socket...";
$service_port = 8888;
$address = gethostbyname('localhost');
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
    echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
} else {
    echo "OK.\n";
}

echo ">>> Attempting to connect to '$address' on port '$service_port'...";
$result = socket_connect($socket, $address, $service_port);
if ($result === false) {
    echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
} else {
    echo "OK.\n";
}

$requests = array();
$requests[] = <<< EOS
{
  "method": "geonlp.parse",
  "params":
    [
      "NIIは千代田区一ツ橋２－１－２にあります。一部は千葉県千葉市にもあります。", { "geocoding":true }
    ],
  "id": "1"
}
EOS;

$requests[] = <<< EOS
{
  "method": "geonlp.parse",
  "params":
    [
      "株式会社情報試作室は多摩市永山１－５にあります。", { "geocoding":true }
    ],
  "id": "2"
}
EOS;

foreach ($requests as $i => $request) {
  $request .= "{EOR}";

  echo ">>> Sending request...";
  socket_write($socket, $request, strlen($request));
  echo "OK.\n";

  echo ">>> Reading response:\n";
  $out = '';
  while ($out .= socket_read($socket, 2048)) {
    if (preg_match('/{EOR}/', $out)) {
      echo $out . "\n";
      break;
    }
  }
}

echo ">>> Closing socket...";
socket_close($socket);
echo "OK.\n";
