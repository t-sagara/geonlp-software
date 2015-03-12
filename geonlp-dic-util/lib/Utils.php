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
  echo $message;
  @\ob_flush();
  \flush();
}
