<?php
namespace GeoNLP;
require_once(dirname(__FILE__).'/lib/LocalRepository.php');
require_once(dirname(__FILE__).'/lib/Utils.php');

define('DEFAULT_GEONLP_SERVER', 'https://geonlp.ex.nii.ac.jp/api/dictionary');
$geonlp_server = DEFAULT_GEONLP_SERVER;
$geonlp_proxy_server = null;

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
    $server = null;
    if (array_key_exists('server', $options)) {
        $server = $options['server'];
    } else {
        $server = \getenv('GEONLP_SERVER');
    }
    if ($server) {
        if (substr($server, strlen($server) -1, 1) == '/') {
            $server = substr($server, 0, strlen($server) - 1);
        }
        $GLOBALS['geonlp_server'] = $server;
        write_message(sprintf("アクセスする GeoNLP サーバを '%s' に変更しました．\n", $server));
    }
    // GeoNLP プロキシサーバの設定
    $proxy = null;
    if (array_key_exists('proxy', $options)) {
        $proxy = $options['proxy'];
    } else {
        $proxy = \getenv('GEONLP_PROXY_SERVER');
    }
    if ($proxy) {
        $GLOBALS['geonlp_proxy_server'] = $proxy;
        write_message(sprintf("プロキシサーバを '%s' に変更しました．\n", $proxy));
    }

    return array('options'=>$options, 'params'=>$params);
}

function usage() {
    $default_data_dir = LocalRepository::get_default_data_dir();
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

    compile パラメータ不要
         ダウンロード済みの辞書からバイナリ地名辞書を
         コンパイルします．

    install パラメータ不要
         コンパイルしたバイナリ地名辞書をインストール
         （GeoNLP が認識するディレクトリにコピー）します．

    forceprefix 0/1 辞書データ名 [辞書データ名2 ...]
         指定した辞書の prefix を省略禁止(1)または許可(0)します．
         compile を実行するまで反映されません．
         show で状態を確認できます．
         
    forcesuffix 0/1 辞書データ名 [辞書データ名2 ...]
         指定した辞書の suffix を省略禁止(1)または許可(0)します．
         compile を実行するまで反映されません．
         show で状態を確認できます．
         

オプション：
    --cc    キャッシュをクリアします．
         list, sync に影響します．

    --dir=<ディレクトリ>
         ローカルリポジトリの場所を指定します．
         デフォルトは {$default_data_dir} です．

         環境変数 GEONLP_DIR で指定することもできます．

    --creator=<データ作成者ニックネーム>
         指定したデータ作成者による辞書のみを対象とします．
         デフォルトは未指定で，全ての作成者による辞書が
         対象となります．

         list, sync で機能します．

         環境変数 GEONLP_CREATOR で指定することもできます．

    --subject=<固有名クラス>
         指定したる固有名クラスを含む辞書のみを対象とします．
         デフォルトは未指定で，全ての辞書が対象となります．

         list, sync で機能します．

    --server=<GeoNLP公開サーバの URL プレフィックス>
         地名辞書をダウンロードするための公開サーバの URL を
         指定します。デフォルトは以下の URL です。

         https://geonlp.ex.nii.ac.jp/api/dictionary

         環境変数 GEONLP_SERVER で指定することもできます．

    --proxy=<外部ネットワークへのProxyサーバのURL>
         外部ネットワークに接続するためにプロキシサーバを
         利用する必要がある場合は，その URL を指定します。
         デフォルトは未設定でダイレクトに接続します。
         認証が必要なプロキシサーバには対応しておりません。

         環境変数 GEONLP_PROXY_SERVER で指定することもできます．

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
    $dics = $local->getDictionaries();
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
        die();
    }
    $dics = $local->getDictionaries();
    for ($i = 1; $i < count($args['params']); $i++) {
        $name = $args['params'][$i];
        if (array_key_exists($name, $dics)) {
            $dic = $dics[$name];
            // 更新状態
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

            // prefix, suffix の強制状態
            $forced = $local->isForcedPrefixSuffix($dic);
            $messages = array();
            if (isset($forced['prefix'])) {
                $messages[]= 'Prefix 省略不可';
            } else {
                $messages[]= 'Prefix 省略可';
            }
            if (isset($forced['suffix'])) {
                $messages[]= 'Suffix 省略不可';
            } else {
                $messages[]= 'Suffix 省略可';
            }
            if (count($messages) > 0) {
                write_message(sprintf("個別設定：\t%s\n", implode(', ', $messages)));
            }

            // 辞書情報
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

/**
 * forceprefix
 * prefix の省略を許さない
 **/
function geonlp_util_forceprefix($args) {
    $local = new LocalRepository($args);
    if (count($args['params']) < 3) {
        write_message("prefix の強制フラグと削除する辞書名を指定してください．\n例1： php geonlp-dic-util.php forceprefix 1 geonlp/world_country\n例2： php geonlp-dic-util.php forceprefix 0 '/geonlp\/.*/'\n強制フラグ：強制する（省略を許可しない）場合は1、強制しない（省略を許可する）場合は0");
        die();
    }
    $dics = $local->getDictionaries();
    $forced = array();
    foreach ($dics as $dicname => $dic) {
        for ($i = 2; $i < count($args['params']); $i++) {
            $pattern = $args['params'][$i];
            if (!array_key_exists($pattern, $forced)) {
                $forced[$pattern] = array();
            }
            if (preg_match('!^/.*/$!', $pattern)) {
                // 正規表現で指定されている
                if (@preg_match($pattern, $dicname)) {
                    $local->force_prefix($dic, $args[1]);
                    $forced[$pattern] []= $dicname;
                    break;
                }
            } else { // 完全一致
                if (\strtolower($pattern) == \strtolower($dicname)) {
                    $local->force_prefix($dic, $args[1]);
                    $forced[$pattern] []= $dicname;
                }
            }
        }
    }
    foreach ($forced as $pattern => $forced_dics) {
        if (count($forced_dics) == 0) {
            write_message("辞書 '".$pattern."' が見つかりません．\n", array("status"=>"warning"));
        }
    }
}

/**
 * forcesuffix
 * suffix の省略を許さない
 **/
function geonlp_util_forcesuffix($args) {
    $local = new LocalRepository($args);
    if (count($args['params']) < 3) {
        write_message("suffix の強制フラグと削除する辞書名を指定してください．\n例1： php geonlp-dic-util.php forcesuffix 1 geonlp/world_country\n例2： php geonlp-dic-util.php forcesuffix 0 '/geonlp\/.*/'\n強制フラグ：強制する（省略を許可しない）場合は1、強制しない（省略を許可する）場合は0");
        die();
    }
    $dics = $local->getDictionaries();
    $forced = array();
    foreach ($dics as $dicname => $dic) {
        for ($i = 2; $i < count($args['params']); $i++) {
            $pattern = $args['params'][$i];
            if (!array_key_exists($pattern, $forced)) {
                $forced[$pattern] = array();
            }
            if (preg_match('!^/.*/$!', $pattern)) {
                // 正規表現で指定されている
                if (@preg_match($pattern, $dicname)) {
                    $local->force_suffix($dic, $args[1]);
                    $forced[$pattern] []= $dicname;
                    break;
                }
            } else { // 完全一致
                if (\strtolower($pattern) == \strtolower($dicname)) {
                    $local->force_suffix($dic, $args['params'][1]);
                    $forced[$pattern] []= $dicname;
                }
            }
        }
    }
    foreach ($forced as $pattern => $forced_dics) {
        if (count($forced_dics) == 0) {
            write_message("辞書 '".$pattern."' が見つかりません．\n", array("status"=>"warning"));
        }
    }
}



// go everything from here
main();
