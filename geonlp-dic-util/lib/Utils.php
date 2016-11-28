<?php
namespace GeoNLP;

// @file Utils.php
// @description 汎用関数群

/**
 * メッセージを出力する
 * @param $message    出力する文字列
 * @param $options    出力オプション（未使用）
 */
function write_message($message, $options = array()) {
  if (isset($options['status'])) {
    echo '['.$options['status'].'] ';
  }
  echo $message;
  @\ob_flush();
  \flush();
}

/**
 * stream_context を作成する
 * @param なし
 * @return context オブジェクト
 */
function get_stream_context() {
  // Proxy サーバ対応
  if (isset($GLOBALS['geonlp_proxy_server']) && $GLOBALS['geonlp_proxy_server']) {
    $proxy = $GLOBALS['geonlp_proxy_server'];
    $options = array('http' => array(
      'proxy' => $proxy,
      'request_fulluri' => true,
    ));
  } else {
    $options = array('http' => array(
      'request_fulluri' => true,
    ));
  }
  return stream_context_create($options);
}
