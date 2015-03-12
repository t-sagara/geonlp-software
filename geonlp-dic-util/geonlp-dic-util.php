<?php
namespace GeoNLP;
require_once(dirname(__FILE__).'/lib/LocalRepository.php');
require_once(dirname(__FILE__).'/lib/Utils.php');

define('DEFAULT_GEONLP_SERVER', 'https://geonlp.ex.nii.ac.jp/api/dictionary');
$geonlp_server = DEFAULT_GEONLP_SERVER;

function get_arguments() {
  $options = array();
  $params  = array();

  global $argv;
  for ($i = 1; $i < count($argv); $i++) {
    if (preg_match('/^\-\-(.+)=(.+)$/', $argv[$i], $matches)) {
      $options[$matches[1]] = $matches[2];
    } else if (preg_match('/^\-\-([^=]+)$/', $argv[$i], $matches)) {
      $options[$matches[1]] = true;
    } else {
      $params []= $argv[$i];
    }
  }
  // GeoNLP サーバの設定
  if (array_key_exists('server', $options)) {
    if (substr($options['server'], strlen($options['server']) -1, 1) == '/') {
      $options['server'] = substr($options['server'], 0, strlen($options['server']) - 1);
    }
    write_message(sprintf("サーバを '%s' に変更しました．\n", $options['server']));
    $GLOBALS['geonlp_server'] = $options['server'];
  }
  return array('options'=>$options, 'params'=>$params);
}

function usage() {
  $msg =<<< _USAGE_
使い方： php geonlp-dic-util [オプション] コマンド <パラメータ>

説明：
  GeoNLP で利用可能な地名解析辞書（以下「辞書」）を管理する
  ユーティリティです．公開サーバからのダウンロードや、
  非公開の CSV ファイルを登録する処理を行います．

コマンド：
  list    パラメータ不要
          GeoNLP公開サーバ上の最新辞書情報を取得し、
          簡易一覧表示します．

  show    辞書データ名 [辞書データ名2 ...]
          指定した辞書の詳細情報を表示します．

  sync    パラメータ不要
          GeoNLP公開サーバからダウンロード済みの辞書を
          最新の状態に更新します．
  
  add     辞書データ名 [辞書データ名2 ...]
          指定した辞書をダウンロードして登録します．

  delete  辞書データ名 [辞書データ名2 ...]
          指定した辞書を即時削除します．

  import  JSONファイル CSVファイル（URLも可）
   または 辞書コード CSVファイル（URLも可）
          CSV ファイルから辞書を作成します．
          既存の辞書コードを指定した場合は上書きします．

  compile ダウンロード済みの辞書からバイナリ地名辞書を
          コンパイルします．

  install コンパイルしたバイナリ地名辞書をインストール
          （GeoNLP が認識するディレクトリにコピー）します．

オプション：
  --cc    キャッシュをクリアします．
          list, sync に影響します．

  --dir=<ディレクトリ>
          ローカルリポジトリの場所を指定します．
          デフォルトは $(HOME)/.geonlp-dic-util/ です．

  --creator=<データ作成者ニックネーム>
          指定したデータ作成者による辞書のみを対象とします．
          list, sync で機能します．

  --subject=<固有名クラス正規表現>
          指定した正規表現に一致する固有名クラスを含む
          辞書のみを対象とします．
          list, sync で機能します．

  --server=<GeoNLP公開サーバの URL プレフィックス>
          デフォルト: https://geonlp.ex.nii.ac.jp/api/dictionary
          外部ネットワークに接続するためにプロキシサーバを
          利用する必要がある場合などは，上記 URL に該当する
          URL プレフィックスを指定してください．

_USAGE_;
  write_message($msg);
}

// main
function main() {
  $args = get_arguments();
  if (count($args['params']) < 1) {
    usage();
    die();
  }
  $command = '\GeoNLP\geonlp_util_'.$args['params'][0];
  if (is_callable($command)) {
    try {
      $result = $command($args);
    } catch (\RuntimeException $e) {
      write_message(sprintf("以下の例外が発生しました．\n『%s』\nエラーの箇所はファイル'%s'の%d行目です．\n", $e->getMessage(), $e->getFile(), $e->getLine()));
      exit(0);
    }
  } else {
    usage();
    die();
  }
}

/**
 * list
 * 簡易一覧表示
 **/
function geonlp_util_list($args) {
  $local = new LocalRepository($args);
  if (count($args['params']) == 1) {
    $dics = $local->getDictionaries();
  } else {
    write_message("パラメータは無視されます．\n", array("status"=>"warning"));
  }
  foreach ($dics as $id => $dic) {
    if (!$dic->isMatchConditions($args['options'])) {
      continue;
    }
    $status = $local->check($dic);
    if ($status == 'need update') {
      $st = '要更新';
    } else if ($status == 'updated') {
      $st = '最新';
    } else if ($status == 'not installed') {
      $st = '未登録';
    } else {
      $st = 'ローカル';
    }
    write_message(sprintf("%s\t%s\t%s\n", $st, $id, $dic->getTitle()));
  }
}

/**
 * show
 * 詳細表示
 **/
function geonlp_util_show($args) {
  $local = new LocalRepository($args);
  if (count($args['params']) < 2) {
    write_message("表示する辞書名を指定してください．\n例： php geonlp-dic-util.php show geonlp/world_country\n");
    usage(); die();
  }
  $dics = $local->getDictionaries();
  for ($i = 1; $i < count($args['params']); $i++) {
    $name = $args['params'][$i];
    if (array_key_exists($name, $dics)) {
      $dic = $dics[$name];
      $status = $local->check($dic);
      if ($status == 'need update') {
	$st = '要更新（公開サーバに、より新しい辞書があります）';
      } else if ($status == 'updated') {
	$st = '最新（ダウンロード済みの辞書が最新です）';
      } else if ($status == 'not installed') {
	$st = '未登録（この辞書はダウンロードされていません）';
      } else {
	$st = 'ローカル（この辞書は公開サーバにありません）';
      }
      write_message(sprintf("状態：\t%s\n", $st));
      write_message($dic->info());
    } else {
      write_message("辞書 '".$name."' が見つかりません．\n", array("status"=>"warning"));
    }
  }
}

/**
 * sync
 * 更新
 **/
function geonlp_util_sync($args) {
  $local = new LocalRepository($args);
  $dics = $local->getDictionaries();
  foreach ($dics as $name => $dic) {
    if (!$dic->isMatchConditions($args['options'])) {
      continue;
    }
    $status = $local->check($dic);
    if ($status == 'need update') {
      $local->install($dic);
    } else if ($status == 'updated') {
      write_message(sprintf("辞書 '%s' は更新済みです．\n", $name));
    }
  }
}

/**
 * add
 * ダウンロードインストール、または更新
 **/
function geonlp_util_add($args) {
  $local = new LocalRepository($args);
  if (count($args['params']) < 2) {
    write_message("追加する辞書名を指定してください．\n例1： php geonlp-dic-util.php add geonlp/world_country\n例2： php geonlp-dic-util.php add '/geonlp\/.*/'\n");
    die();
  }
  $dics = $local->getDictionaries();
  $installed = array();
  foreach ($dics as $dicname => $dic) {
    for ($i = 1; $i < count($args['params']); $i++) {
      $pattern = $args['params'][$i];
      if (!array_key_exists($pattern, $installed)) {
	$installed[$pattern] = array();
      }
      if (preg_match('!^/.*/$!', $pattern)) {
	// 正規表現で指定されている
	if (@preg_match($pattern, $dicname)) {
	  $local->install($dic);
	  $installed[$pattern] []= $dicname;
	  break;
	}
      } else { // 完全一致
	if (\strtolower($pattern) == \strtolower($dicname)) {
	  $local->install($dic);
	  $installed[$pattern] []= $dicname;
	}
      }
    }
  }
  foreach ($installed as $pattern => $installed_dics) {
    if (count($installed_dics) == 0) {
      write_message("辞書 '".$pattern."' が見つかりません．\n", array("status"=>"warning"));
    }
  }
}

/**
 * delete
 * ダウンロード辞書の削除（論理削除）
 **/
function geonlp_util_delete($args) {
  $local = new LocalRepository($args);
  if (count($args['params']) < 2) {
    write_message("削除する辞書名を指定してください．\n例1： php geonlp-dic-util.php delete geonlp/world_country\n例2： php geonlp-dic-util.php delete '/geonlp\/.*/'\n");
    die();
  }
  $dics = $local->getDictionaries();
  $deleted = array();
  foreach ($dics as $dicname => $dic) {
    for ($i = 1; $i < count($args['params']); $i++) {
      $pattern = $args['params'][$i];
      if (!array_key_exists($pattern, $deleted)) {
	$deleted[$pattern] = array();
      }
      if (preg_match('!^/.*/$!', $pattern)) {
	// 正規表現で指定されている
	if (@preg_match($pattern, $dicname)) {
	  $local->delete($dic);
	  $deleted[$pattern] []= $dicname;
	  break;
	}
      } else { // 完全一致
	if (\strtolower($pattern) == \strtolower($dicname)) {
	  $local->delete($dic);
	  $deleted[$pattern] []= $dicname;
	}
      }
    }
  }
  foreach ($deleted as $pattern => $deleted_dics) {
    if (count($deleted_dics) == 0) {
      write_message("辞書 '".$pattern."' が見つかりません．\n", array("status"=>"warning"));
    }
  }
}

/**
 * compile
 * ダウンロード済辞書からバイナリ地名辞書をコンパイルする
 **/
function geonlp_util_compile($args) {
  $local = new LocalRepository($args);
  $dics = $local->getDictionaries();
  $install_dics = array();
  foreach ($dics as $name => $dic) {
    if (!$dic->isMatchConditions($args['options'])) {
      continue;
    }
    $status = $local->check($dic);
    if ($status != 'not installed') {
      $install_dics []= $dic;
    }
  }
  $local->compile($install_dics);
}

/**
 * install
 * コンパイルしたバイナリ地名辞書をインストールする
 **/
function geonlp_util_install($args) {
  $local = new LocalRepository($args);
  $local->install_binary();
}

/**
 * import
 * 独自のローカル CSV をインポートする
 **/
function geonlp_util_import($args) {
  if (count($args['params']) != 3) {
    write_message("辞書コードと CSV ファイル名を指定してください．\n例： php geonlp-dic-util.php import mylocation foo.csv\n");
    die();
  }
  $local = new LocalRepository($args);
  $local->importDictionary($args['params'][1], $args['params'][2]);
}

// go everything from here
main();
