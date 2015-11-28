<?php
namespace GeoNLP;

// @file GeoNLPDictionary.php
// @description GeoNLP 地名辞書のクラス

class GeoNLPDictionary
{
  // コンストラクタ
  function __construct($properties = NULL) {
    if (!$properties) {
      $this->properties = self::defaultDictionaryInfo();
    } else {
      $this->properties = $properties;
    }
    $this->data = array();
  }

  // デフォルト地名辞書情報
  public static function defaultDictionaryInfo() {
    // https://geonlp.ex.nii.ac.jp/docs/common/csv_format.html#json
    return array(
		 "identifier"=>"",
		 "internal_id"=>-1,
		 "creator"=>"local",
		 "title"=>"no title",
		 "description"=>"",
		 "source"=>"",
		 "spatial"=>array(array(0,0),array(0,0)),
		 "subject"=>array(),
		 "issued"=>"",
		 "modified"=>"",
		 "report_count"=>0,
		 "record_count"=>0,
		 "icon"=>"",
		 "url"=>"",
		 );
  }

  // この辞書を一意に表す文字列 (creator/name) を取得する
  public function getIdString() {
    $id = $this->getIdentifier();
    if ($id) {
      $id_args = explode('/', $id);
      $id = $id_args[count($id_args) -2].'/'.$id_args[count($id_args) -1];
    }
    return $id;
  }
  
  public function getIdentifier() {
    if (array_key_exists('identifier', $this->properties)) {
      return $this->properties['identifier'];
    }
    return FALSE;
  }

  public function getInternalId() {
    if (array_key_exists('internal_id', $this->properties)) {
      return $this->properties['internal_id'];
    }
    return FALSE;
  }
  
  public function getUrl() {
    if (array_key_exists('url', $this->properties)) {
      return $this->properties['url'];
    }
    return FALSE;
  }
  
  public function getTitle() {
    if (array_key_exists('title', $this->properties)) {
      return $this->properties['title'];
    }
    return FALSE;
  }

  public function getModified() {
    if (array_key_exists('modified', $this->properties)) {
      return $this->properties['modified'];
    }
    return FALSE;
  }

  public function getProperty() {
    return $this->properties;
  }

  // パス名を生成する
  public function getRepositoryPath() {
    $id = $this->getInternalId();
    if ($id < 0) {
      $path = 'minus'.abs($id);
    } else {
      $path = $id;
    }
    return $path;
  }

  public function prefixGeonlpId() {
    $id = $this->getInternalId();
    if ($id < 0) {
      $prefix = '_n'.abs($id);
    } else {
      $prefix = '_p'.$id;
    }
    return $prefix;
  }

  // geonlp_id をランダムに生成する
  public function generateGeonlpId() {
    $geonlp_id = $this->prefixGeonlpId();
    for ($i = 0; $i < 4; $i++) {
      $r = rand(0, 62);
      if ($r < 10) {
	$c = chr(ord('0') + $r);
      } else if ($r < 36) {
	$c = chr(ord('a') + $r - 10);
      } else {
	$c = chr(ord('A') + $r - 36);
      }
      $geonlp_id .= $c;
    }
    return $geonlp_id;
  }

  public function setModified($time) {
    $this->properties['modified'] = $time;
  }
  
  // 指定した検索条件に一致するか判定する
  // @param $options 検索条件を示す連想配列
  //   'creator'   作成者名，部分一致
  //   'subject'   収録固有名クラス，正規表現
  public function isMatchConditions($options) {
    if (array_key_exists('creator', $options)) {
      $pattern = '/'.$options['creator'].'/i';
      if (!preg_match($pattern, $this->properties['creator'])
	  && !preg_match($pattern, $this->getIdString())) {
	return false;
      }
    }
    if (array_key_exists('subject', $options)) {
      $ismatch = false;
      foreach ($this->properties['subject'] as $subject) {
	if (preg_match($options['subject'], $subject)) {
	  $ismatch = true;
	  break;
	}
      }
      if (!$ismatch) return false;
    }
    return true;
  }

  public function info() {
    $info  = "識別子  ：\t".$this->properties['identifier']."\n";
    $info .= "内部ID  ：\t".$this->properties['internal_id']."\n";
    $info .= "作成者  ：\t".$this->properties['creator']."\n";
    $info .= "辞書名  ：\t".$this->properties['title']."\n";
    if (array_key_exists('source', $this->properties)) {
      $info .= "情報ソース：\t".$this->properties['source']."\n";
    }
    $info .= "修正報告件数：\t".$this->properties['report_count']."\n";
    $info .= "公開日時：\t".$this->properties['issued']."\n";
    $info .= "修正日時：\t".$this->properties['modified']."\n";
    if (array_key_exists('icon', $this->properties)) {
      $info .= "アイコン画像URL：\t".$this->properties['icon']."\n";
    }
    $info .= "辞書URL ：\t".$this->properties['url']."\n";
    $info .= "空間範囲：\t".json_encode($this->properties['spatial'])."\n";
    $info .= "収録固有名クラス：\t".implode(', ', $this->properties['subject'])."\n";
    $info .= "辞書説明 ---------------\n".$this->properties['description']."\n";
    return $info;
  }

  /**
   * 最新の辞書データを公開サーバから取得する
   * @param 辞書IDをキーとする GeoNLPDictionary の連想配列
   */
  static public function getDictionariesFromRepository($uri = NULL) {
    if (is_null($uri)) $uri = $GLOBALS['geonlp_server'];
    $response = @file_get_contents($uri);
    if (!$response) {
      write_message("GeoNLP サーバから辞書一覧を取得できませんでした．\n", array("status"=>"warning"));
      return array();
    }
    $r = json_decode($response, true);
    $dicts = array();
    foreach ($r['dictionaries'] as $d) {
      $dict = new GeoNLPDictionary($d);
      $dicts[$dict->getIdString()] = $dict;
    }
    return $dicts;
  }

}
