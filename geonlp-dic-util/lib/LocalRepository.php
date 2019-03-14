<?php
namespace GeoNLP;
use \RuntimeException;
require_once(dirname(__FILE__).'/Utils.php');
require_once(dirname(__FILE__).'/GeonlpDictionary.php');

// @file LocalRepository.php
// @description ローカル地名辞書リポジトリの管理クラス

class LocalRepository
{
    // コンストラクタ
    function __construct($args = array("options"=>array(), "params"=>array())) {
        $this->args = $args;
        $dir = (array_key_exists('dir', $args['options'])) ? $args['options']['dir'] : '';
        $this->set_wd($dir);
        $this->open_db();
        if (array_key_exists('cc', $args['options'])) { // キャッシュクリア
            write_message("キャッシュをクリアします．\n");
            $this->cache_clear();
        }
    }

    // GeoNLP のデフォルト data_dir を取得する
    static public function get_default_data_dir() {
        $output = array();
        @\exec("geonlp_ma_makedic -h", $output);
        $usage = implode("\n", $output);
        // 旧バージョン対応
        // sqlite3_file  = /usr/local/lib/geonlp/geodic.sq3
        if (preg_match('/sqlite3_file\s*=\s*(.*)\/geodic.sq3/', $usage, $m)) {
            return $m[1];
        }
        write_message("GeoNLP のデフォルト辞書ディレクトリが取得できません．\nGeoNLP がインストールされていて， geonlp_ma_makedic コマンドが\n実行可能であることを確認してください．", array("status"=>"error"));
        die();
    }

    // GeoNLP のデフォルト conf_dir を取得する
    static public function get_default_conf_dir() {
        $output = array();
        @\exec("geonlp_ma_makedic -h", $output);
        $usage = implode("\n", $output);
        if (preg_match('/geonlp_rc\s*=\s*(.*)\/geonlp.rc/', $usage, $m)) {
            return $m[1];
        }
        write_message("GeoNLP のデフォルト設定ディレクトリが取得できません．\nGeoNLP がインストールされていて， geonlp_ma_makedic コマンドが\n実行可能であることを確認してください．", array("status"=>"error"));
        die();
    }

    // リポジトリを置くディレクトリをセットする
    private function set_wd($dir) {
        if ($dir) {
            ; // コマンドラインオプション --dir で指定された値を使う
        } else {
            $dir = \getenv('GEONLP_DIR'); // 環境変数から取得する
            if (!$dir) {
                // 環境変数も未設定の場合, GeoNLP インストール時のデフォルト値
                $dir = self::get_default_data_dir();
            }
        }
        if (substr($dir, -1) == '/') {
            $dir = substr($dir, 0, strlen($dir) -1);
        }
        $wd = $dir.'/.geonlp-dic-util';
        if (\file_exists($wd)) {
            if (!\is_dir($wd) || !\is_writable($wd)) {
                throw new RuntimeException(sprintf("ローカルリポジトリ用ディレクトリ（'%s'）が存在しますが，ディレクトリではないか、書き込みが許可されていません．\n", $wd));
            }
        } else {
            write_message(sprintf("ローカルリポジトリを %s に作成します．\n", $wd));
            if (! @\mkdir($wd, 0755, true)) {
                write_message(sprintf("ディレクトリの作成に失敗しました．書き込みが許可されているか確認してください．\n"), array("status"=>"error"));
                write_message(sprintf("オプション --dir ，環境変数 GEONLP_DIR で別のディレクトリを指定することもできます．\n"), array("status"=>"hint"));
                die();
            }
            \mkdir($wd.'/zip', 0755);
            \mkdir($wd.'/extracted', 0755);
        }
        $this->geonlp_dir = $dir; // バイナリ辞書をインストールするディレクトリ
        $this->wd = $wd; // 作業用ディレクトリ
    }

    // データベースをセットする
    private function open_db() {
        if (isset($this->db)) {
            $this->db = null;
        }
        $this->dsn = 'sqlite:'.$this->wd.'/repository.sq3';
        $this->db = new \PDO($this->dsn);

        // テーブルが存在しない場合は作成する
        // $this->db->exec("CREATE TABLE IF NOT EXISTS dictionary(id INTEGER PRIMARY KEY, name TEXT, updated INTEGER, enabled INTEGER, jsonfile TEXT, csvfile TEXT, json TEXT)");
        $this->db->exec("CREATE TABLE IF NOT EXISTS dictionary(id INTEGER PRIMARY KEY, name TEXT, updated INTEGER, enabled INTEGER, force_prefix INTEGER, force_suffix INTEGER, jsonfile TEXT, csvfile TEXT, json TEXT)");

        // dictionary テーブルのバージョンを確認する
        $check_columns = array("force_prefix" => true, "force_suffix" => true);
        $sth = $this->db->prepare("PRAGMA table_info(dictionary)");
        $sth->execute();
        while ($r = $sth->fetch(\PDO::FETCH_ASSOC)) {
            if ($r['name'] == 'force_prefix') {
                unset($check_columns['force_prefix']);
            } else if ($r['name'] == 'force_suffix') {
                unset($check_columns['force_suffix']);
            }
        }
        if (isset($check_columns['force_prefix'])
            || isset($check_columns['force_suffix'])) { // 辞書の更新が必要
            write_message("- 辞書管理データベースをバージョンアップしています ...");
            $this->db->exec("CREATE TABLE tmp_dictionary AS SELECT id, name, updated, enabled, 0 as force_prefix, 0 as force_suffix, jsonfile, csvfile, json FROM dictionary");
            $this->db->exec("DROP TABLE dictionary");
            $this->db->exec("ALTER TABLE tmp_dictionary RENAME TO dictionary");
            write_message("完了.\n");
        }
    }

    // internal id （数値）を指定して登録済み地名辞書情報を取得する
    private function get_dicinfo_by_id($id) {
        $sth = $this->db->prepare("SELECT * FROM dictionary WHERE id = :id");
        $sth->bindParam(':id', $id);
        $sth->execute();
        $result = $sth->fetch(\PDO::FETCH_ASSOC);
        return $result;
    }

    // internal id （数値）を指定して登録済み地名辞書情報を利用可能（不能）にする
    private function enable_dicinfo_by_id($id, $enable=1) {
        $sth = $this->db->prepare("UPDATE dictionary SET enabled = :enabled WHERE id = :id");
        $sth->bindParam(':enabled', $enable);
        $sth->bindParam(':id', $id);
        $sth->execute();
        $result = $sth->fetch(\PDO::FETCH_ASSOC);
        return $result;
    }
    
    // internal id （数値）を指定して登録済み地名辞書の prefix を省略可能（不能）にする
    private function force_prefix_by_id($id, $enable=1) {
        $sth = $this->db->prepare("UPDATE dictionary SET force_prefix = :enabled WHERE id = :id");
        $sth->bindParam(':enabled', $enable);
        $sth->bindParam(':id', $id);
        $sth->execute();
        $result = $sth->fetch(\PDO::FETCH_ASSOC);
        return $result;
    }
    
    // internal id （数値）を指定して登録済み地名辞書の suffix を省略可能（不能）にする
    private function force_suffix_by_id($id, $enable=1) {
        $sth = $this->db->prepare("UPDATE dictionary SET force_suffix = :enabled WHERE id = :id");
        $sth->bindParam(':enabled', $enable);
        $sth->bindParam(':id', $id);
        $sth->execute();
        $result = $sth->fetch(\PDO::FETCH_ASSOC);
        return $result;
    }
    

    // 名前（文字列）を指定して登録済み地名辞書情報を取得する
    private function get_dicinfo_by_name($name) {
        $sth = $this->db->prepare("SELECT * FROM dictionary WHERE name = :name");
        $sth->bindParam(':name', $name);
        $sth->execute();
        $result = $sth->fetch(\PDO::FETCH_ASSOC);
        return $result;
    }

    // 新しく登録するローカル地名辞書の内部IDを計算する
    private function get_local_id($name) {
        $dicinfo = $this->get_dicinfo_by_name($name);
        if ($dicinfo) {
            $min = $dicinfo['id'];
            if ($min < 0) return (int)$min;
            return FALSE; // ローカルではない辞書の名前と一致する
        }
        $sth = $this->db->prepare("SELECT min(id) FROM dictionary");
        $sth->execute();
        $min = $sth->fetchColumn();
        if (!$min || $min > 0) {
            $min = -1;
        } else {
            $min = $min - 1;
        }
        return (int)$min;
    }

    // 登録済み地名辞書情報の一覧を取得する
    public function get_dicinfo_list() {
        $dictionary_list = array();
        // クエリ
        $sth = $this->db->prepare("SELECT * FROM dictionary ORDER BY id ASC");
        $sth->execute();
        while ($r = $sth->fetch(\PDO::FETCH_ASSOC)) {
            $dictionary_list []= $r;
        }
        return $dictionary_list;
    }

    // 地名辞書情報から地名辞書オブジェクトを生成する
    private function get_dictionary_from_dicinfo($r) {
        $dic = new GeoNLPDictionary(json_decode($r['json'], true));
        return $dic;
    }

    // 地名辞書をインストールする
    // リモートリポジトリからダウンロードする
    // 最終更新日時のチェックは行わない
    public function install(GeoNLPDictionary $dic) {
        $id = $dic->getInternalId();
        if ($id > 0) { // 公開データ
            write_message(sprintf("辞書（タイトル：'%s'）を公開サーバから取得します．\n", $dic->getTitle()));
            // Zip ファイルをダウンロードして保存する
            write_message(sprintf("- ZIP ファイルをダウンロードしています ..."));
            $url = $dic->getUrl();
            if ($GLOBALS['geonlp_server'] != DEFAULT_GEONLP_SERVER) {
                $url = preg_replace('!'.DEFAULT_GEONLP_SERVER.'!', $GLOBALS['geonlp_server'], $url);
            }
            $zipcontent = \file_get_contents($url, false, get_stream_context());
            $zipfile = $this->wd.'/zip/'.$id.'.zip';
            write_message(sprintf("\n- ファイル '%s' に保存します．\n", $zipfile));
            $file = \fopen($zipfile, 'w');
            \fwrite($file, $zipcontent);
            \fclose($file);
            // Zip ファイルを展開する
            $jsonfile = '';
            $csvfile  = '';
            $extract_dir = $this->wd.'/extracted/'.$dic->getRepositoryPath().'/';
            write_message(sprintf("- ZIP ファイルを '%s' に展開します．\n", $extract_dir));
            $zip = new \ZipArchive();
            $zip->open($zipfile);
            $zip->extractTo($extract_dir);
            for ($i = 0; $i < $zip->numFiles; $i++) {
                $entry = $zip->statIndex($i);
                if (preg_match('/\.json$/i', $entry['name'])) {
                    $jsonfile = $entry['name'];
                } else if (preg_match('/\.csv$/i', $entry['name'])) {
                    $csvfile = $entry['name'];
                }
            }
            $zip->close();
            // JSON ファイルの情報を読み込む
            $json = file_get_contents($extract_dir.$jsonfile);
            // データベースに登録する
            write_message(sprintf("- ローカルリポジトリに登録します．\n"));
            $updated = time();
            $name = $dic->getIdString();
            $sth = $this->db->prepare("INSERT OR IGNORE INTO dictionary (id, name, updated, enabled, jsonfile, csvfile, json) VALUES (:id, :name, :updated, 1, :jsonfile, :csvfile, :json)");
            $sth->bindParam(':id', $id);
            $sth->bindParam(':name', $name);
            $sth->bindParam(':updated', $updated);
            $sth->bindParam(':jsonfile', $jsonfile);
            $sth->bindParam(':csvfile', $csvfile);
            $sth->bindParam(':json', $json);
            $sth->execute();
            $sth = $this->db->prepare("UPDATE dictionary SET name = :name, updated = :updated, enabled = 1, jsonfile = :jsonfile, csvfile = :csvfile, json = :json WHERE id = :id");
            $sth->bindParam(':id', $id);
            $sth->bindParam(':name', $name);
            $sth->bindParam(':updated', $updated);
            $sth->bindParam(':jsonfile', $jsonfile);
            $sth->bindParam(':csvfile', $csvfile);
            $sth->bindParam(':json', $json);
            $sth->execute();
            write_message(sprintf("完了.\n"));
        } else {
            write_message("この辞書はローカル辞書です．\n", array("status"=>"warning"));
        }
    }

    // 地名辞書を削除する
    // データベース上の enabled を 0 にセットするだけで
    // 物理削除は行わない
    public function delete(GeoNLPDictionary $dic) {
        $id = $dic->getInternalId();
        $dic_installed = $this->get_dicinfo_by_id($id);
        if (!$dic_installed) {
            write_message(sprintf("辞書（タイトル：'%s'）は登録されていません．\n", $dic->getTitle()), array("status"=>"warning"));
        } else {
            $this->enable_dicinfo_by_id($id, 0); // disable
            write_message(sprintf("辞書（タイトル：'%s'）を削除しました．\n", $dic->getTitle()));
        }
    }

    // 地名辞書の prefix を省略可・不可にする
    public function force_prefix(GeoNLPDictionary $dic, $flag) {
        $id = $dic->getInternalId();
        $dicinfo = $this->get_dicinfo_by_id($id);
        if (!$dicinfo) {
            write_message(sprintf("辞書（タイトル：'%s'）は登録されていません．\n", $dic->getTitle()), array("status"=>"warning"));
        } else if (!$dicinfo['enabled']) {
            write_message(sprintf("辞書（タイトル：'%s'）は削除されています．\n", $dic->getTitle()), array("status"=>"warning"));
        } else {
            $this->force_prefix_by_id($id, $flag);
            if ($flag) {
                write_message(sprintf("辞書（タイトル：'%s'）の prefix を省略不可にしました．\n", $dic->getTitle()));
            } else {
                write_message(sprintf("辞書（タイトル：'%s'）の prefix を省略可にしました．\n", $dic->getTitle()));
            }
            write_message(sprintf("次に complie を実行する時に反映されます．\n"));
        }
    }

    // 地名辞書の suffix を省略可・不可にする
    public function force_suffix(GeoNLPDictionary $dic, $flag) {
        $id = $dic->getInternalId();
        $dicinfo = $this->get_dicinfo_by_id($id);
        if (!$dicinfo) {
            write_message(sprintf("辞書（タイトル：'%s'）は登録されていません．\n", $dic->getTitle()), array("status"=>"warning"));
        } else if (!$dicinfo['enabled']) {
            write_message(sprintf("辞書（タイトル：'%s'）は削除されています．\n", $dic->getTitle()), array("status"=>"warning"));
        } else {
            $this->force_suffix_by_id($id, $flag);
            if ($flag) {
                write_message(sprintf("辞書（タイトル：'%s'）の suffix を省略不可にしました．\n", $dic->getTitle()));
            } else {
                write_message(sprintf("辞書（タイトル：'%s'）の suffix を省略可にしました．\n", $dic->getTitle()));
            }
            write_message(sprintf("次に complie を実行する時に反映されます．\n"));
        }
    }

    // 地名辞書に更新が必要かどうかチェックする
    public function check(GeoNLPDictionary $dic) {
        $id = $dic->getInternalId();
        if ($id > 0) {
            $localinfo = $this->get_dicinfo_by_id($id);
            if (!$localinfo || !$localinfo['enabled']) {
                return 'not installed';
            }

            $modified = $dic->getModified();
            if (!$modified) {
                write_message("この辞書には最終更新タイムスタンプの情報がありません．", array("status"=>"warning"));
            } else {
                // 辞書の更新日時（文字列, 2013-06-07 18:24:11形式）をタイムスタンプに変換する
                $m = strptime($modified, "%Y-%m-%d %H:%M:%S");
                if ($m['tm_year'] < 1900) $m['tm_year'] += 1900;
                $modified = @\mktime($m['tm_hour'], $m['tm_min'], $m['tm_sec'], $m['tm_mon'], $m['tm_mday'], $m['tm_year']);
                // データベースの更新時刻を確認する
                $dictionary_info = $this->get_dicinfo_by_id($id);
                if ($dictionary_info['updated'] > $modified + 5) {
                    return 'updated';
                }
            }
            return 'need update';
        }
        return 'local';
    }

    // 地名辞書の prefix, suffix が強制されているかどうか
    public function isForcedPrefixSuffix(GeoNLPDictionary $dic) {
        $id = $dic->getInternalId();
        $result = array();
        $localinfo = $this->get_dicinfo_by_id($id);
        if (!$localinfo || !$localinfo['enabled']) {
            return null; // インストールされていない辞書
        }
        if ($localinfo['force_prefix']) {
            $result['prefix'] = true;
        }
        if ($localinfo['force_suffix']) {
            $result['suffix'] = true;
        }
        return $result;
    }

    // 地名辞書を更新する
    // 最終更新日時のチェックを行い、
    // 更新されていればリモートリポジトリからダウンロードする
    public function update(GeoNLPDictionary $dic) {
        $status = $this->check($dic);
        if ($status != 'need update') {
            write_message("この辞書は更新の必要がありません．\n");
        } else {
            // ダウンロード・インストールを行う
            $this->install($dic);
        }
    }

    // 地名辞書一覧を作成する
    public function getDictionaries() {
        // 公開サーバの辞書を取得する
        $dictionary_list = $this->wd.'/dictionary_list.json';
        if (!file_exists($dictionary_list)
            || time() - filemtime($dictionary_list) > 3600) {
            write_message(sprintf("GeoNLP サーバ '%s' から最新の辞書一覧を取得しています．\n", $GLOBALS['geonlp_server']));
            $response = @file_get_contents($GLOBALS['geonlp_server'], false, get_stream_context());
            if ($response !== FALSE) {
                file_put_contents($dictionary_list, $response);
            } else {
                write_message("サーバから辞書一覧を取得できませんでした．\n", array("status"=>"warning"));
            }
        }
        $dicts = GeoNLPDictionary::getDictionariesFromRepository($dictionary_list);

        // ローカルリポジトリの辞書を取得して上書きする
        foreach ($this->get_dicinfo_list() as $r) {
            if (!$r['enabled']) continue;
            $dict = $this->get_dictionary_from_dicinfo($r);
            $id = $dict->getIdString();
            $dicts[$id] = $dict;
        }
        return $dicts;
    }

    // キャッシュファイルをクリアする
    private function cache_clear() {
        $dictionary_list = $this->wd.'/dictionary_list.json';
        @unlink($dictionary_list);
    }

    // バイナリ地名辞書をコンパイルする
    public function compile($dics) {
        if (count($dics) == 0) {
            write_message("辞書が1つも登録されていません．\n", array("status"=>"error"));
            write_message("先に add コマンドで辞書を登録してください．\n", array("status"=>"hint"));
            return;
        }
        $geodic = $this->wd.'/geodic.sq3';
        $rc     = $this->wd.'/geonlp.rc';
        $ma_rc  = $this->wd.'/geonlp_ma_makedic.rc';
        $init_gz = dirname(__FILE__).'/../resource/geodic-init.dump.gz';
        $mecab_libexec_dir = exec("mecab-config --libexecdir", $output, $retval);   // mecab-dict-index の場所を特定
        if ($retval != 0) {
            write_message("mecab-config が見つからないため、処理を中止します。", array("status"=>"error"));
            die();
        }
        // 初期設定ファイルを配置
        $data_dir = self::get_default_data_dir();
        $conf_dir = self::get_default_conf_dir();
        $pattern = sprintf("!%s!u", preg_quote($data_dir));
        // geonlp.rc
        $geonlp_rc = file_get_contents($conf_dir.'/geonlp.rc');
        $geonlp_rc = preg_replace($pattern, $this->wd, $geonlp_rc);
        file_put_contents($rc, $geonlp_rc);
        // geonlp_ma_makedic.rc
        $geonlp_ma_makedic = file_get_contents($conf_dir.'/geonlp_ma_makedic.rc');
        $geonlp_ma_makedic = preg_replace($pattern, $this->wd, $geonlp_ma_makedic);
        file_put_contents($ma_rc, $geonlp_ma_makedic);
        @unlink($geodic);
        $cmd = sprintf("gzip -dc %s | sqlite3 %s", $init_gz, $geodic);
        write_message(`$cmd`);
        // 辞書を追加
        foreach ($dics as $dic) {
            $id = $dic->getInternalId();
            $dicinfo = $this->get_dicinfo_by_id($id);
            $dir = $this->wd.'/extracted/'.$dic->getRepositoryPath();
            $csvfile = $dir.'/'.$dicinfo['csvfile'];
            $jsonfile = $dir.'/'.$dicinfo['jsonfile'];
            if (!is_readable($jsonfile)) {
                write_message(sprintf("辞書 '%s' の JSON ファイルが '%s' に見つかりません．", $dic->getTitle(), $dir->path), array("status"=>"warning"));
                continue;
            }
            if (!is_readable($csvfile)) {
                write_message(sprintf("辞書 '%s' の CSV ファイルが '%s' に見つかりません．", $dic->getTitle(), $dir->path), array("status"=>"warning"));
                continue;
            }
            // prefix, suffix の省略を禁止した場合、CSVを編集する

            $tmp_csvfile = $this->ApplyPrefixSuffixCsv($csvfile, $dicinfo);

            // 辞書データの追加
            $cmd = sprintf("cd %s; geonlp_add --rc=%s %s %s", $this->wd, $rc, $jsonfile, $tmp_csvfile);
            write_message(sprintf("辞書 '%s' のデータを読み込みます．\n", $dic->getTitle()));
            $descriptorspec = array(0=>array("pipe", "r"),
                                    1=>array("pipe", "w"),
                                    2=>array("pipe", "w"));
            $proc = proc_open($cmd, $descriptorspec, $pipes);
            if (is_resource($proc)) {
                $stdout = stream_get_contents($pipes[1]);
                $stderr = stream_get_contents($pipes[2]);
                fclose($pipes[1]);
                fclose($pipes[2]);
                $ret = proc_close($proc);
                if ($ret != 0) {
                    write_message("エラーが発生しました．\n".$stderr);
                    throw new RuntimeException("辞書の読み込みに失敗．");
                }
            }
            if ($tmp_csvfile != $csvfile) {
                unlink($tmp_csvfile);
            }
            //`${cmd}`;
        }
        // コンパイル
        $cmd = sprintf("cd %s; geonlp_ma_makedic -u -f %s %s", $this->wd, $ma_rc, $rc);
        write_message("バイナリ地名辞書をコンパイルしています．\n");
        `${cmd}`;
        write_message("完了しました．\n");
    }

    // force_prefix, force_suffix オプションに合わせて修正した CSV を生成する
    private function ApplyPrefixSuffixCsv($csvfile, $dicinfo) {
        if (!$dicinfo['force_prefix'] && !$dicinfo['force_suffix']) {
            return $csvfile; // 修正不要
        }
        setlocale(LC_ALL, 'ja_JP.UTF-8');

        // 出力先ファイルを生成
        $dirname = dirname($csvfile);
        $basename = basename($csvfile);
        $new_csvfile = $dirname . '/_' . $basename;
        $ofp = fopen($new_csvfile, "w");

        // 見出し行から prefix と suffix の位置を取得
        $prefix_field = null;
        $suffix_field = null;
        $ifp = fopen($csvfile, "r");
        $index_fields = fgetcsv($ifp);
        if ($dicinfo['force_prefix']) {
            for ($i = 0; $i < count($index_fields); $i++) {
                if ($index_fields[$i] == 'prefix') {
                    $prefix_field = $i;
                    break;
                }
            }
        }
        if ($dicinfo['force_suffix']) {
            for ($i = 0; $i < count($index_fields); $i++) {
                if ($index_fields[$i] == 'suffix') {
                    $suffix_field = $i;
                    break;
                }
            }
        }
        fputcsv($ofp, $index_fields);

        // データ行をチェック
        while ($data = fgetcsv($ifp)) {
            if ($prefix_field) {
                // prefix に含まれる空（省略可能）要素を除去する
                $values = explode('/', $data[$prefix_field]);
                $values = array_filter($values, 'strlen');
                $data[$prefix_field] = implode('/', $values);
            }
            if ($suffix_field) {
                // suffix に含まれる空（省略可能）要素を除去する
                $values = explode('/', $data[$suffix_field]);
                $values = array_filter($values, 'strlen');
                $data[$suffix_field] = implode('/', $values);
            }
            fputcsv($ofp, $data);
        }
        fclose($ifp);
        fclose($ofp);

        return $new_csvfile;
    }


    // バイナリ地名辞書をインストールする
    public function install_binary() {
        // インストール先は data_dir
        // インストールするファイル一覧を取得
        $usage = `geonlp_ma_makedic -h`;
        $paths = array();
        $pattern = '!(.*)\s*=\s*.*/(.*\..+)!';
        foreach (explode("\n", $usage) as $line) {
            if (preg_match($pattern, $line, $m)) {
                $paths[$m[1]] = $m[2];
            }
        }
        write_message("以下のファイルをインストールします．\n");
        $total_error = array("unreadable"=>0, "unwritable"=>0);
        foreach ($paths as $title => $filename) {
            $srcfile = $this->wd.'/'.$filename;
            $destfile = $this->geonlp_dir.'/'.$filename;
            if (!is_readable($srcfile)) {
                write_message(sprintf("インストールするファイル %s が存在しません．\n", $srcfile), array("status"=>"error"));
                $total_error['unreadable']++;
            } else {
                $srctime = @strftime("%Y-%m-%d %H:%M:%S", filemtime($srcfile));
                if (file_exists($destfile)) {
                    $desttime = @strftime("%Y-%m-%d %H:%M:%S", filemtime($destfile));
                } else {
                    $desttime = '新しいファイル';
                }
                write_message(sprintf("%s(%s)\n-> %s(%s)\n", $srcfile, $srctime, $destfile, $desttime), array("status"=>"info"));
            }
            if (!file_exists($destfile)) {
                if (!is_writable(dirname($destfile))) {
                    write_message(sprintf("インストール先ディレクトリ %s は書き込み不可です\n", dirname($destfile)), array("status"=>"error"));
                    $total_error['unwritable']++;
                }
            }
            if (file_exists($destfile) && !is_writable($destfile)) {
                write_message(sprintf("インストール先ファイル %s は書き込み不可です\n", $destfile), array("status"=>"error"));
                $total_error['unwritable']++;
            }
        }
        if ($total_error['unreadable'] > 0) {
            write_message("先に compile を実行し，バイナリ地名辞書を作成してください．\n", array("status"=>"hint"));
        }
        if ($total_error['unwritable'] > 0) {
            write_message("インストール先ファイルを書き込み許可にするか，書き込み権限のあるユーザで実行してください．\n", array("status"=>"error"));
        }
        if ($total_error['unreadable'] > 0 || $total_error['unwritable'] > 0) return;

        write_message("古いファイルは上書きされます．よろしいですか？[y/n] ");
        $in = trim(fgets(STDIN, 1024));
        if (strtolower($in) != 'y') return;

        // コピー手順を出力
        foreach ($paths as $title => $filename) {
            $srcfile = $this->wd.'/'.$filename;
            $destfile = $this->geonlp_dir.'/'.$filename;
            \copy($srcfile, $destfile);
        }

        // プロファイルファイル
        /*
          $profile_path = $this->geonlp_dir.'/geonlp.rc';
          if (!file_exists($profile_path)) {
          $data_dir = self::get_default_data_dir();
          $pattern = sprintf("!%s!u", preg_quote($data_dir));
          $rc = file_get_contents(self::get_default_conf_dir().'/geonlp.rc');
          $new_rc = preg_replace($pattern, $this->geonlp_dir, $rc);
          file_put_contents($profile_path, $new_rc);
          write_message(sprintf("プロファイル '%s' を作成しました．\n", $profile_path));
          }
          $profile_path = $this->geonlp_dir.'/geonlp_ma_makedic.rc';
          if (!file_exists($profile_path)) {
          $data_dir = self::get_default_data_dir();
          $pattern = sprintf("!%s!u", preg_quote($data_dir));
          $rc = file_get_contents(self::get_default_conf_dir().'/geonlp_ma_makedic.rc');
          $new_rc = preg_replace($pattern, $this->geonlp_dir, $rc);
          file_put_contents($profile_path, $new_rc);
          write_message(sprintf("プロファイル '%s' を作成しました．\n", $profile_path));
          }
        */
        write_message("完了しました．\n");
    }

    /**
     * CSV ファイルから辞書を作成する
     * 既存の辞書コードを指定した場合は上書きする
     */
    public function importDictionary($code, $csvfile) {
        $dic = NULL;
        $dicinfo = NULL;
        if (file_exists($code) && is_readable($code)) {
            // JSON ファイル？
            $json = file_get_contents($code);
            $dicinfo = @json_decode($json, TRUE);
        }
        if (!is_null($dicinfo)) { // JSON ファイルを読み込んだ
            $dic = new GeoNLPDictionary($dicinfo);
            $name = $dic->getIdString();
            if (!$name) {
                throw new RuntimeException(sprintf("JSON ファイル '%s' 中，Identifier のフォーマットが正しくありません．", $code));
            }
        } else {
            if (!preg_match('/^[0-9A-Za-z\-_]{1,32}$/', $code)) {
                throw new RuntimeException(sprintf("辞書コードは英数字，ハイフン，アンダーバーからなる32文字以内の文字列で指定してください．"));
            }
            $name = 'local/'.$code;
        }
        // 辞書コードが確定
        $new_id = $this->get_local_id($name);
        if ($new_id === FALSE) {
            $dic = $this->get_dictionary_from_dicinfo($dicinfo);
            throw new RuntimeException(sprintf("辞書 '%s' はローカル辞書ではないため上書きできません．", $dic->getTitle()));
        }
        // 新規登録
        if (!$dic) {
            $properties = GeoNLPDictionary::defaultDictionaryInfo();
            $properties['identifier'] = $name;
            $properties['internal_id'] = $new_id;
            $properties['title'] = $code;
            $dic = new GeoNLPDictionary($properties);
        }
        // 辞書情報（JSON）の設定
        if (!$dicinfo) $dicinfo = $dic->getProperty();
        $dic->setModified(@strftime("%Y-%m-%d %H:%M:%S", time()));

        // CSV の読み込み
        $csv = file_get_contents($csvfile);
        if (!$csv) {
            throw new RuntimeException(sprintf("CSV ファイル '%s' が読み込めません．", $csvfile));
        }
        $tmpfile = tmpfile();
        fwrite($tmpfile, $csv);
        fseek($tmpfile, 0, SEEK_SET);
        $header = fgetcsv($tmpfile);
        // GeoNLP ID フィールドを追加する
        $geonlp_id_field = -1;
        $entity_id_field = -1;
        for ($i = 0; $i < count($header); $i++) {
            if ($header[$i] == 'geonlp_id') {
                $geonlp_id_field = $i;
            } else if ($header[$i] == 'entity_id') {
                $entity_id_field = $i;
            }
        }
        if ($entity_id_field < 0) {
            throw new RuntimeException(sprintf("CSV ファイル '%s' に entity_id フィールドが存在しません．", $csvfile));
        }
        // JSON ファイル出力
        $extract_dir = $this->wd.'/extracted/'.$dic->getRepositoryPath();
        @mkdir($extract_dir);
        $path_name = str_replace('/', '-', $name);
        $jsonfile = $extract_dir.'/'.$path_name.'.json';
        $json = json_encode($dicinfo);
        file_put_contents($jsonfile, $json);

        // CSV ファイル出力
        $csvfile = $extract_dir.'/'.$path_name.'.csv';
        $file = fopen($csvfile, 'w');
        if ($geonlp_id_field < 0) {
            // geonlp_id フィールドを持たない CSV の場合
            array_unshift($header, 'geonlp_id');
            $records = array("header"=>true);
            fputcsv($file, $header);
            // GeoNLP ID を生成
            while (($record = fgetcsv($tmpfile))) {
                while (true) {
                    $new_geonlp_id = $dic->generateGeonlpId();
                    if (!array_key_exists($new_geonlp_id, $records)) break;
                }
                array_unshift($record, $new_geonlp_id);
                $records[$new_geonlp_id] = true;
                fputcsv($file, $record);
            }
        } else {
            // geonlp_id フィールドを持つ場合
            $prefix = $dic->prefixGeonlpId();
            $prefix_len = strlen($prefix);
            // 一度全ての geonlp_id を記憶する
            while (($record = fgetcsv($tmpfile))) {
                $geonlp_id = $record[$geonlp_id_field];
                if (substr($geonlp_id, 0, $prefix_len) == $prefix) {
                    $records[$geonlp_id] = true;
                }
            }
            // geonlp_id を発行しなおす
            fseek($tmpfile, 0, SEEK_SET);
            $header = fgetcsv($tmpfile);
            fputcsv($file, $header);
            while (($record = fgetcsv($tmpfile))) {
                $geonlp_id = $record[$geonlp_id_field];
                if (substr($geonlp_id, 0, $prefix_len) == $prefix) {
                    // この geonlp_id は再利用する
                    $records[$geonlp_id] = true;
                } else {
                    // 新しい geonlp_id を生成する
                    while (true) {
                        $new_geonlp_id = $dic->generateGeonlpId();
                        if (!array_key_exists($new_geonlp_id, $records)) break;
                    }
                    $record[$geonlp_id_field] = $new_geonlp_id;
                    $records[$new_geonlp_id] = true;
                }
                fputcsv($file, $record);
            }
        }
        fclose($file);

        // データベースに登録
        $jsonfile = $path_name.'.json';
        $csvfile = $path_name.'.csv';
        write_message(sprintf("- ローカルリポジトリに登録します．\n"));
        $updated = time();
        $sth = $this->db->prepare("INSERT OR IGNORE INTO dictionary (id, name, updated, enabled, jsonfile, csvfile, json) VALUES (:id, :name, :updated, 1, :jsonfile, :csvfile, :json)");
        $sth->bindValue(':id', $new_id);
        $sth->bindValue(':name', $name);
        $sth->bindValue(':updated', $updated);
        $sth->bindValue(':jsonfile', $jsonfile);
        $sth->bindValue(':csvfile', $csvfile);
        $sth->bindValue(':json', $json);
        $sth->execute();
        $sth = $this->db->prepare("UPDATE dictionary SET name = :name, updated = :updated, enabled = 1, jsonfile = :jsonfile, csvfile = :csvfile, json = :json WHERE id = :id");
        $sth->bindValue(':id', $new_id);
        $sth->bindValue(':name', $name);
        $sth->bindValue(':updated', $updated);
        $sth->bindValue(':jsonfile', $jsonfile);
        $sth->bindValue(':csvfile', $csvfile);
        $sth->bindValue(':json', $json);
        $sth->execute();
        write_message(sprintf("完了.\n"));
    }
}

// ユニットテスト
if (isset($argv) && $argv[0] == basename(__FILE__)) {
    $local = new LocalRepository();
    print_r($local->get_dicinfo_by_id(1));
    print_r($local->get_dicinfo_by_id(2));
    die();
    $dictionaries = GeoNLPDictionary::getDictionariesFromRepository();
    $dictionary = $dictionaries['geonlp/world_country'];
    $local->update($dictionary);
}
