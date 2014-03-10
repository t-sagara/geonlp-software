///
/// @file
/// @brief  地名語解決 API の実装
/// @author 株式会社情報試作室
///
/// Copyright (c)2013, NII
///

#include <config.h>
#include <string.h>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "GeonlpService.h"
#include "Profile.h"

namespace geonlp
{

  // 配列で初期化した picojson::value オブジェクトを作成する
  static picojson::value _v_array(const picojson::array varray) {
    picojson::value va;
    const char* s = "[]";
    picojson::parse(va, s, s + strlen(s));
    va.get<picojson::array>() = varray;
    return va;
  }

  // サービスを作成する
  // ジオコーダも初期化する
  ServicePtr createService(const std::string& profile) throw (ServiceCreateFailedException)
  {
    // Profileの読み込み
    boost::shared_ptr<Profile> profilesp = boost::shared_ptr<Profile>(new Profile());
    std::string profilepath = Profile::searchProfile(profile);
    try {
      profilesp->load(profilepath);
    } catch (std::runtime_error& e) {
      throw ServiceCreateFailedException(e.what(), ServiceCreateFailedException::PROFILE);
    }
		
    // MA を作成する
    MAPtr ma_ptr = createMA(profile); 
    
#ifdef HAVE_LIBDAMS
    // ジオコーダを初期化する
    
    // プロファイルから DAMS ファイルのパスを取得する
    std::string damsfile = profilesp->get_damsfile();
    try {
      damswrapper::init(damsfile);
    } catch (damswrapper::DamsInitException& e) {
      throw ServiceCreateFailedException (e.what(), ServiceCreateFailedException::DAMS);
    }
#endif /* HAVE_LIBDAMS */

    try {
      ServicePtr servicep = ServicePtr(new Service(ma_ptr, profilesp));
      return servicep;
    } catch (std::runtime_error& e) {
      throw ServiceCreateFailedException(e.what(), ServiceCreateFailedException::SERVICE);
    }
    
  }

  /// GeonlpService の実装

  /// @brief JSON-RPC のリクエストを受け取って実行する
  /// @param @c json_request  リクエストオブジェクト
  /// @return JSON-RPC のレスポンスオブジェクト
  picojson::value Service::proc(const picojson::value& json_request) {
    std::string method;
    picojson::array params;
    picojson::value id;
    picojson::ext response;
    
    try {

      // フォーマットチェック
      if (!json_request.is<picojson::object>())
	throw ServiceRequestFormatException();
      picojson::object request_obj = json_request.get<picojson::object>();

      if (request_obj.count("method") == 0)
	throw ServiceRequestFormatException("'method' must not be empty.");
      if (request_obj.count("params") == 0)
	throw ServiceRequestFormatException("'params' must not be empty.");
      if (request_obj.count("id") == 0)
	throw ServiceRequestFormatException("'id' must not be empty.");

      if (!request_obj["method"].is<std::string>()) {
	throw ServiceRequestFormatException("'method' must be a string.");
      } else {
	method = request_obj["method"].get<std::string>();
	if (method.substr(0, 7) == "geonlp.") {
	  method = method.substr(7);
	}
      }

      if (!request_obj["params"].is<picojson::array>()) {
	throw ServiceRequestFormatException("'params' must be an array.");
      } else {
	params = request_obj["params"].get<picojson::array>();
      }

      id = request_obj["id"];
    
      // 処理実行（ディスパッチ）
      picojson::value result;
      if (method == "version") {
	result = this->version(params);
      } else if (method == "parse") {
	result = this->parse(params);
      } else if (method == "parseStructured") {
	result = this->parseStructured(params);
      } else if (method == "search") {
	result = this->search(params);
      } else if (method == "getGeoInfo") {
	result = this->getGeoInfo(params);
      } else if (method == "getDictionaries") {
	result = this->getDictionaries(params);
      } else if (method == "getDictionaryInfo") {
	result = this->getDictionaryInfo(params);
#ifdef HAVE_LIBDAMS
      } else if (method == "addressGeocoding") {
	result = this->addressGeocoding(params);
#endif /* HAVE_LIBDAMS */	
      } else {
	throw ServiceRequestFormatException("Unknown or not implemented method called.");
      }

      // レスポンスの作成
      response.set_value("result", result);
      response.set_value("error", picojson::value());
      response.set_value("id", id);

    } catch (std::runtime_error& e) {
      
      // 例外発生 - エラーレスポンスの作成
      response.set_value("result", picojson::value());
      response.set_value("error", e.what());
      response.set_value("id", id);

    }

    return response;
  }

  // parse オプションのセット
  void Service::set_options(const picojson::value& options)
    throw (picojson::PicojsonException, ServiceRequestFormatException) {
    // 型チェック
    if (!options.is<picojson::object>() && !options.is<picojson::null>())
      throw ServiceRequestFormatException("Option format is invalid.");
    picojson::ext op(options);

    // オプション
    if (op.has_key("geocoding")) {
      picojson::value v = op.get_value("geocoding");
      std::string geocoding_type;
      if (v.is<bool>()) {
	if (v.get<bool>()) geocoding_type = "normal";
	else geocoding_type = "none";
      } else if (v.is<std::string>()) {
	geocoding_type = op._get_string("geocoding");
      }
      try {
	this->_options.set_value("geocoding", geocoding_type);
	//this->_options.set_value("geocoding", op._get_bool("geocoding"));
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"geocoding\" must be a string value.");
      }
    }
    
    if (op.has_key("adjunct")) {
      try {
	this->_options.set_value("adjunct", op._get_bool("adjunct"));
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"adjunct\" must be a boolean value.");
      }
    }
    
    if (op.has_key("threshold")) {
      try {
	this->_options.set_value("threshold", op._get_int("threshold"));
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"threshold\" must be an int value.");
      }
    }

    if (op.has_key("show-score")) {
      try {
	this->_options.set_value("show-score", op._get_bool("show-score"));
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"show-score\" must be a boolean value.");
      }
    }

    if (op.has_key("show-candidate")) {
      try {
	this->_options.set_value("show-candidate", op._get_bool("show-candidate"));
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"show-candidate\" must be a boolean value.");
      }
    }

    if (op.has_key("set-dic")) {
      try {
	std::vector<int> dics = op._get_int_list("set-dic");
	this->_ma_ptr->setActiveDictionaries(dics);
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"set-dic\" must be an array of int value.");
      }
    }

    if (op.has_key("add-dic")) {
      try {
	std::vector<int> dics = op._get_int_list("add-dic");
	this->_ma_ptr->addActiveDictionaries(dics);
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"add-dic\" must be an array of int value.");
      }
    }

    if (op.has_key("remove-dic")) {
      try {
	std::vector<int> dics = op._get_int_list("remove-dic");
	this->_ma_ptr->removeActiveDictionaries(dics);
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"remove-dic\" must be an array of int value.");
      }
    }

    if (op.has_key("set-class")) {
      try {
	std::vector<std::string> classes = op._get_string_list("set-class");
	this->_ma_ptr->setActiveClasses(classes);
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"set-class\" must be an array of string value.");
      }
    }

    if (op.has_key("add-class")) {
      try {
	std::vector<std::string> current_classes = this->_ma_ptr->getActiveClasses();
	std::vector<std::string> classes = op._get_string_list("add-class");
	for (std::vector<std::string>::iterator it = classes.begin(); it != classes.end(); it++) {
	  std::string check = std::string("-") + (*it);
	  bool is_erased = false;
	  for (std::vector<std::string>::iterator it2 = current_classes.begin(); it2 != current_classes.end(); it2++) {
	    if (*it2 == check) { // 除外クラスとして登録されているので削除する
	      is_erased = true;
	      current_classes.erase(it2);
	      break;
	    }
	  }
	  if (! is_erased) { // 未登録なのでクラスとして追加登録する
	    current_classes.push_back((*it));
	  }
	}
	this->_ma_ptr->setActiveClasses(current_classes);
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"add-class\" must be an array of string value.");
      }
    }

    if (op.has_key("remove-class")) {
      try {
	std::vector<std::string> current_classes = this->_ma_ptr->getActiveClasses();
	std::vector<std::string> classes = op._get_string_list("remove-class");
	for (std::vector<std::string>::iterator it = classes.begin(); it != classes.end(); it++) {
	  bool is_erased = false;
	  for (std::vector<std::string>::iterator it2 = current_classes.begin(); it2 != current_classes.end(); it2++) {
	    if (*it2 == *it) {
	      is_erased = true;
	      current_classes.erase(it2);
	      break;
	    }
	  }
	  if (! is_erased) { // 未登録なので除外クラスとして追加登録する
	    current_classes.push_back(std::string("-") + (*it));
	  }
	}
	
	this->_ma_ptr->setActiveClasses(current_classes);
      } catch (picojson::PicojsonException& e) {
	throw ServiceRequestFormatException("Option \"remove-class\" must be an array of string value.");
      }
    }

    // コンテキストにもオプションをセット
    this->_context.setOptions(this->_options);
  }

  // parse オプションのリセット
  void Service::reset_options(void) {
    this->_options.set_value("geocoding", "normal");
    this->_options.set_value("adjunct", false);
    this->_options.set_value("threshold", 0);
    this->_options.set_value("show-score", false);
    this->_options.set_value("show-candidate", false);
    this->_ma_ptr->resetActiveDictionaries();
    this->_ma_ptr->resetActiveClasses();
    this->_context.setOptions(this->_options);
  }

  /// 一文のジオパース処理
  /// コンテキスト、オプションはクラスの状態のまま
  picojson::value Service::parse_sentence(const std::string& sentence) {
    this->queue_sentence(sentence);
    this->resolve();
    return this->dequeue_sentence();
  }

  /// 一文をジオパース処理後、コンテキストキューに積む
  /// コンテキスト、オプションはクラスの状態のまま
  void Service::queue_sentence(const std::string& sentence) {
    double probability;
    picojson::ext result;
    picojson::value null;
    std::vector<Node> nodes;
    this->_ma_ptr->parseNode(sentence, nodes);
    picojson::array varray;
    std::string nog_sentence = "";
    std::vector<Node>::iterator pre_node = nodes.end();

    for (std::vector<Node>::iterator it = nodes.begin();
	 it != nodes.end();
	 pre_node = it, it++) {
      probability = 1.0;
      Node node = *it;
      std::string surface = node.get_surface();
      if (surface == "") continue; // BOS, EOS をスキップ

      // 地名修飾語のチェック
      if (!this->_options._get_bool("adjunct") && node.get_conjugatedForm() == "名詞-固有名詞-地名修飾語") {
	nog_sentence += surface;
	continue;
      }
      
      // 人名のチェック1 姓名が連続している場合は人名とみなす
      if (node.get_conjugatedForm() == "名詞-固有名詞-人名-姓" || (node.get_partOfSpeech() == "名詞" && node.get_subclassification1() == "固有名詞" && node.get_subclassification2() == "人名") && it + 1 != nodes.end()) {
	std::vector<Node>::iterator next_node = it + 1;
	if ((*next_node).get_conjugatedForm() == "名詞-固有名詞-人名-名" || ((*next_node).get_partOfSpeech() == "名詞" && (*next_node).get_subclassification1() == "固有名詞" && (*next_node).get_subclassification2() == "人名")) {
	  nog_sentence += surface + next_node->get_surface();
	  it++;
	  continue;
	}
      }
      // 人名のチェック2 名詞,接尾,人名の前の名詞は人名とみなす
      if (node.get_partOfSpeech() == "名詞" && it + 1 != nodes.end()) {
	std::vector<Node>::iterator next_node = it + 1;
	if ((*next_node).get_partOfSpeech() == "名詞" && (*next_node).get_subclassification1() == "接尾" && (*next_node).get_subclassification2() == "人名") {
	  nog_sentence += surface + next_node->get_surface();
	  it++;
	  continue;
	}
      }
      // 人名のチェック3 名詞, 名詞,接尾,人名の前の名詞は人名とみなす
      if (node.get_partOfSpeech() == "名詞" && it + 1 != nodes.end() && it + 2 != nodes.end()) {
	std::vector<Node>::iterator next_node = it + 1;
	std::vector<Node>::iterator nnext_node = it + 2;
	if ((*nnext_node).get_partOfSpeech() == "名詞" && (*nnext_node).get_subclassification1() == "接尾" && (*nnext_node).get_subclassification2() == "人名" && (*next_node).get_partOfSpeech() == "名詞") {
	  nog_sentence += surface + next_node->get_surface() + nnext_node->get_surface();
	  it += 2;
	  continue;
	}
      }

      // 年号のチェック 明治/大正/昭和/平成-数字/記号-年/年度/年代
      if (surface == "明治" || surface == "大正" || surface == "昭和" || surface == "平成") {
	bool is_era = false;
	std::string new_surface = surface;
	for (std::vector<Node>::iterator it_era = it + 1; it_era != nodes.end(); it_era++) {
	  std::string s = (*it_era).get_surface();
	  new_surface += s;
	  if (s == "年" || s == "年度" || s == "年代" || s == "元年" || (*it_era).get_partOfSpeech() == "記号") {
	    nog_sentence += new_surface;
	    it = it_era;
	    is_era = true;
	    break;
	  } else if ((*it_era).get_subclassification1() != "数") {
	    break;
	  }
	}
	if (is_era) continue;
      }

      // 上記以外の、他品詞と区別がつかない場合のチェック
      if (node.get_conjugatedForm().length() > 1) {
	probability = this->_classifier.check(nodes, (std::vector<Node>::const_iterator)it);
      }

      // 一般名詞に続く語は地名ではない
      /*
      if (pre_node != nodes.end() && pre_node->get_partOfSpeech() == "名詞") {
	nog_sentence += surface;
	continue;
      }
      */

      // 地名語の処理
      std::map<std::string, Geoword> geowords;
      if (this->_ma_ptr->getGeowordEntries(*it, geowords) > 0) {
	// 地名語の場合
	picojson::ext v("{}");
	  
	if (nog_sentence.length() > 0) {
	  v.set_value("surface", nog_sentence);
	  varray.push_back((picojson::value)v);
	  nog_sentence = "";
	}

#ifdef HAVE_LIBDAMS
	if (this->_options._get_string("geocoding") != "none") {
	  // 住所要素かどうかのチェック
	  bool canBeAddress = false;
	  // boost::regex regexp("^(State|City)(\\/.+|)$"); // State, City, State/..., City/...
	  const boost::regex& regexp = this->_profilesp->get_address_regex();
	  for (std::map<std::string, geonlp::Geoword>::iterator it_geoword = geowords.begin(); it_geoword != geowords.end(); it_geoword++) {
	    Geoword& geoword = (*it_geoword).second;
	    if (boost::regex_match(geoword.get_ne_class(), regexp)) {
	      canBeAddress = true;
	      break;
	    }
	  }
	  if (canBeAddress) {
	    std::vector<geonlp::Address> addresses;
	    std::vector<Node>::iterator it_bak = it;
	    if (this->tryDams(addresses, it, nodes.end())) {
	      // 住所要素が得られた
	      std::string surface("");
	      picojson::array varray_addresses;
	      for (std::vector<geonlp::Address>::iterator it_address = addresses.begin(); it_address != addresses.end(); it_address++) {
		v.initByJson("{}");
		if (surface.length() == 0) surface = (*it_address).get_surface();
		// v.set_value("geo", ((picojson::value)(*it_address).getGeoObject()));
		v.set_value("candidate", (picojson::value)(*it_address));
		varray_addresses.push_back((picojson::value)v);
	      }
	      v.initByJson("{}");
	      v.set_value("surface", surface);
	      v.set_value("address-candidates", (picojson::value)varray_addresses);
	      varray.push_back((picojson::value)v);
	      continue;
	    } else {
	      // 巻き戻して続きから処理をする
	      it = it_bak;
	    }
	  }
	}
#endif /* HAVE_LIBDAMS */

	std::map<std::string, geonlp::Geoword>::iterator it_geoword = geowords.begin();
	// Geoword& geoword = (*it_geoword).second;
	v.initByJson("{}");
	v.set_value("surface", surface);
	// v.set_value("geo", ((picojson::value)geoword.getGeoObject()));
	{ // map::<string, Geoword> を picojson::array に積み替え
	  picojson::array varray_geowords;
	  for (it_geoword = geowords.begin(); it_geoword != geowords.end(); it_geoword++) {
	    varray_geowords.push_back((picojson::value)((*it_geoword).second));
	  }
	  v.set_value("candidates", (picojson::value)varray_geowords);
	}
	if (probability < 1.0) v.set_value("probability", probability);
	varray.push_back((picojson::value)v);
      } else {
	nog_sentence += surface;
      }
    }
    if (nog_sentence.length() > 0) {
      picojson::ext v("{}");
      v.set_value("surface", nog_sentence);
      varray.push_back((picojson::value)v);
      nog_sentence = "";
    }

    // センテンスのエンドマーク
    picojson::ext e("null");
    varray.push_back((picojson::value)e);

    this->_context.addNodes(varray);
    return;
  }
  
  /// キューに積まれている地名を解決する
  void Service::resolve(void) {
    this->_context.evaluate();
  }
  
  /// 一文の地名解決後の結果をコンテキストキューから取得する
  picojson::value Service::dequeue_sentence(void) {
    return _v_array(this->_context.flushNodes());
  }

  /// 地名語検索
  picojson::value Service::version(const picojson::array& params)
    throw (picojson::PicojsonException) {
    picojson::ext result;
    result.set_value("version", std::string(PACKAGE_VERSION));
    return result;
  }

  /// ジオパース処理
  picojson::value Service::parse(const picojson::array& params)
    throw (picojson::PicojsonException, ServiceRequestFormatException) {
    picojson::value options, result;

    // オプションクリア
    this->reset_options();

    // コンテキストクリア
    this->reset_context();

    // パラメータ数チェック
    if (params.size() == 1) {
      ;
    } else if (params.size() == 2) {
      // オプションセット
      this->set_options(params[1]);
    } else {
      throw ServiceRequestFormatException("geonlp.parse accepts 1 or 2 parameters.");
    }

    // パラメータ解析
    if (params[0].is<std::string>()) {
      // 単文のジオパース処理
      result = this->parse_sentence(params[0].get<std::string>());
    } else if (params[0].is<picojson::array>()) {
      // 複数文のジオパース処理
      picojson::array rarray;
      picojson::array sentences = params[0].get<picojson::array>();
      for (std::vector<picojson::value>::iterator it = sentences.begin(); it != sentences.end(); it++) {
	if (!(*it).is<std::string>()) throw ServiceRequestFormatException();
	std::string& str = (*it).get<std::string>();
	this->reset_context();
	picojson::value v = this->parse_sentence(str);
	rarray.push_back(v);
      }
      result = _v_array(rarray);
    }
    return result;
  }

  /// ジオパース処理（タグ付き・複数文）
  picojson::value Service::parseStructured(const picojson::array& params)
    throw (picojson::PicojsonException, ServiceRequestFormatException) {
    picojson::ext options, result, enull("null");
    picojson::value vnull = (picojson::value)enull;

    // オプションクリア
    this->reset_options();

    // コンテキストクリア
    this->reset_context();

    // パラメータ数チェック
    if (params.size() == 1) {
      ;
    } else if (params.size() == 2) {
      // オプションセット
      this->set_options(params[1]);
    } else {
      throw ServiceRequestFormatException("geonlp.parseStructured accepts 1 or 2 parameters.");
    }

    // パラメータ解析
    if (!params[0].is<picojson::array>()) {
      throw ServiceRequestFormatException("The 1st parameter of geonlp.parseStructured must be an array.");
    } else {
      picojson::array params0 = params[0].get<picojson::array>();
      picojson::array rarray;
      for (picojson::array::iterator it = params0.begin(); it != params0.end(); it++) {
	// パラメータ 1 の要素をチェック
	if ((*it).is<std::string>()) { // 文字列要素はキューに積む
	  this->queue_sentence((*it).get<std::string>());
	  rarray.push_back(vnull); // 代わりに null を入れておく
	} else { // それ以外の要素はそのまま返す
	  rarray.push_back(*it);
	}
      }
      this->resolve(); // 地名解決実行
      // 解析結果を戻す
      for (picojson::array::iterator it = rarray.begin(); it != rarray.end(); it++) {
	if (!(*it).is<picojson::null>()) continue;
	const picojson::value& v = this->dequeue_sentence();
	it = rarray.erase(it);
	rarray.insert(it, v);
      }
      
      result = _v_array(rarray);
    }
    return result;
  }

  /// 地名語検索
  picojson::value Service::search(const picojson::array& params)
    throw (picojson::PicojsonException, ServiceRequestFormatException) {
    picojson::ext result;

    // オプションクリア
    this->reset_options();

    // パラメータ数チェック
    if (params.size() == 1) {
      ;
    } else if (params.size() == 2) {
      // オプションセット
      this->set_options(params[1]);
    } else {
      throw ServiceRequestFormatException("geonlp.search accepts 1 or 2 parameters.");
    }

    // パラメータ解析
    if (params[0].is<std::string>()) {
      // 検索する語の表記または読み
      std::string key = params[0].get<std::string>();
      std::map<std::string, Geoword> results;
      this->_ma_ptr->getGeowordEntries(key, results);
      for (std::map<std::string, Geoword>::iterator it = results.begin(); it != results.end(); it++) {
	result.set_value((*it).first, (*it).second);
      }
    } else {
      throw ServiceRequestFormatException("The 1st parameter of geonlp.search must be a string.");
    }
    return result;
  }

  /// @brief geonlp_id から地名語の詳細情報を取得する
  /// @arg @c geonlp_id
  /// @return 検索結果, 見つからなかった場合は error
  picojson::value Service::getGeoInfo(const picojson::array& params) const
    throw (ServiceRequestFormatException) {
    picojson::ext result;
    
    // パラメータ数チェック
    if (params.size() != 1) {
      throw ServiceRequestFormatException("geonlp.getGeoInfo accepts 1 parameter.");
    }
    
    // パラメータ解析
    if (params[0].is<std::string>()) {
      // 1つの geonlp_id から地名語を検索
      std::string geonlp_id = params[0].get<std::string>();
      Geoword geoword;
      if (this->_ma_ptr->getGeowordEntry(geonlp_id, geoword)) {
	result.set_value(geoword.get_geonlp_id(), geoword);
      }
    } else if (params[0].is<picojson::array>()) {
      // 複数の地名語を検索
      picojson::array params0 = params[0].get<picojson::array>();
      for (std::vector<picojson::value>::iterator it = params0.begin(); it != params0.end(); it++) {
	if (!(*it).is<std::string>()) // 文字列以外の配列は受け付けない
	  throw ServiceRequestFormatException("The 1st parameter of geonlp.getGeoInfo must be a string or an array of string.");
	std::string geonlp_id = (*it).get<std::string>();
	Geoword geoword;
	if (this->_ma_ptr->getGeowordEntry(geonlp_id, geoword)) {
	  result.set_value(geoword.get_geonlp_id(), geoword);
	}
      }
    } else {
	  throw ServiceRequestFormatException("The 1st parameter of geonlp.getGeoInfo must be a string or an array of string.");
    }
    return result;
  }

  /// @brief 指定した辞書に関する詳細情報を取得する
  /// @arg なし
  /// @return 辞書のIDをキー、辞書の詳細情報を値とするマップ
  picojson::value Service::getDictionaries(const picojson::array& params) const
    throw (ServiceRequestFormatException) {
    picojson::ext result;

    // パラメータ数チェック
    if (params.size() > 0) {
      throw ServiceRequestFormatException("geonlp.getDictionaries accepts no parameter.");
    }
    
    std::map<int, Dictionary> dictionaries;
    this->_ma_ptr->getDictionaryList(dictionaries);
    for (std::map<int, Dictionary>::iterator it = dictionaries.begin(); it != dictionaries.end(); it++) {
      std::stringstream ss;
      std::string dictionary_id;
      ss << (*it).first;
      ss >> dictionary_id;
      result.set_value(dictionary_id, (*it).second);
    }

    return result;
  }

  /// @brief 指定した辞書に関する詳細情報を取得する
  /// @arg @c params 詳細情報が必要な辞書IDまたは辞書IDの配列
  /// @return 辞書のIDをキー、辞書の詳細情報を値とするマップ
  picojson::value Service::getDictionaryInfo(const picojson::array& params)
    throw (ServiceRequestFormatException) {
    picojson::value param0;
    picojson::ext v, result;

    // パラメータ数チェック
    if (params.size() != 1) {
      throw ServiceRequestFormatException("geonlp.getDictionaryInfo accepts 1 parameter.");
    }
    
    if (params[0].is<long>() || params[0].is<double>()) {
      picojson::array va;
      va.push_back(params[0]);
      param0 = _v_array(va);
    } else if (params[0].is<std::string>()) {
      picojson::array va;
      va.push_back(params[0]);
      param0 = _v_array(va);
    } else if (params[0].is<picojson::array>()) {
      param0 = params[0];
    } else {
      throw ServiceRequestFormatException("geonlp.getDictionaryInfo accepts an integer, a string or an array of integer.");
    }

    picojson::array& parray = param0.get<picojson::array>();
    for (std::vector<picojson::value>::iterator it = parray.begin(); it != parray.end(); it++) {
      Dictionary dictionary;
      int dictionary_id;
      std::string dictionary_id_str;
      if ((*it).is<long>()) {
	dictionary_id = (int)((*it).get<long>());
	std::stringstream ss;
	ss << dictionary_id;
	ss >> dictionary_id_str;
      } else if ((*it).is<double>()) {
	dictionary_id = (int)((*it).get<double>());
	std::stringstream ss;
	ss << dictionary_id;
	ss >> dictionary_id_str;
      } else if ((*it).is<std::string>()) {
	dictionary_id_str = (*it).get<std::string>();
	std::stringstream ss;
	ss << dictionary_id_str;
	ss >> dictionary_id;
      } else {
	throw ServiceRequestFormatException("geonlp.getDictionaryInfo accepts an integer, a string or an array of integer.");
      }
      if (this->_ma_ptr->findDictionaryById(dictionary_id, dictionary)) {
	result.set_value(dictionary_id_str, dictionary);
      }
    }

    return result;
  }

#ifdef HAVE_LIBDAMS
  /// @brief  ジオコーディング結果を Address に積み替える
  /// @arg    address    ジオコーディング結果を格納する Address オブジェクト
  /// @arg    surface    surface に格納すべき文字列
  /// @arg    candidate  DAMS が返すジオコーディング結果（複数存在する場合も一つだけ）
  /// @return 積み込んだ住所要素数（階層の深さ）
  int Service::damsCandidateToAddress(Address& address, const std::string& surface, const damswrapper::Candidate& candidate) const {
    int level, nelement = 0;
    double lat = 0.0, lng = 0.0, x, y;
    picojson::array address_elements;
    picojson::ext address_element;
    std::string standard_form = "";
    std::string name, parent_name("");
    std::string geonlp_id;
    std::string geocoding_type = this->_options._get_string("geocoding");
    const MAPtr ma = this->getMA();

    // 結果を初期化
    address.clear();
    std::map<std::string, std::string> shared_code;
    for (std::vector<damswrapper::AddressElement>::const_iterator it_address = candidate.begin(); it_address != candidate.end(); it_address++) {
      address_element.initByJson("{}");
      name = (*it_address).get_name();
      y = (*it_address).get_y();
      x = (*it_address).get_x();
      if (-90.0 <= y && y <= 90.0) lat = y;
      if (-180.0 <= x && x <= 180.0) lng = x;
      level = (*it_address).get_level();
      std::stringstream ss_level;
      ss_level << level;
      address_element.set_value("name", name);
      address_element.set_value("level", level);
      address_element.set_value("latitude", y);
      address_element.set_value("longitude", x);
      standard_form += (*it_address).get_name();

      if (geocoding_type != "simple") {
	// geonlp_id を名称から逆引き
	geonlp_id = "";
	Geoword geoword;
	std::map<std::string, Geoword> geowords;
	ma->getGeowordEntries(name, geowords);
	for (std::map<std::string, Geoword>::iterator it_geo = geowords.begin(); it_geo != geowords.end(); it_geo++) {
	  if (geonlp_id.length() > 0) break; // 既に見つかった
	  // 住所要素になりうる地名語かどうか、 address_level で判定する
	  if (!(*it_geo).second.has_key("address_level") || 
	      (*it_geo).second._get_string("address_level") != ss_level.str()) continue;

	  if (parent_name.length() == 0) { // 親地名が使えないので先頭の候補を選ぶ
	    geonlp_id = (*it_geo).first;
	    break;
	  }
	  // 親地名が一致する候補を選ぶ
	  std::vector<std::string> hypernyms = (*it_geo).second.get_hypernym();
	  for (std::vector<std::string>::iterator it_hyp = hypernyms.begin(); it_hyp != hypernyms.end(); it_hyp++) {
	    if ((*it_hyp) == parent_name) {
	      geonlp_id = (*it_geo).first;
	      geoword = (*it_geo).second;
	      break;
	    }
	  }
	}
	if (geonlp_id.length() > 0) {
	  address_element.set_value("geonlp_id", geonlp_id);
	  if (geocoding_type == "full") {
	    address_element.set_value("geoword", (picojson::value)geoword);
	  }
	  // 各住所要素の code を累積
	  const std::map<std::string, std::string>& code = geoword.get_code();
	  for (std::map<std::string, std::string>::const_iterator it_code = code.begin(); it_code != code.end(); it_code++) {
	    std::string code_key = (*it_code).first;
	    if (shared_code.count(code_key) > 0) shared_code.erase(code_key);
	    shared_code[code_key] = (*it_code).second;
	  }
	}
      } /* geocoding_type != "simple" */

      address_elements.push_back((picojson::value)address_element);
      nelement++;
      parent_name = name;
    }
    address.set_latitude(lat);
    address.set_longitude(lng);
    address.set_surface(surface);
    address.set_standard_form(standard_form);
    address.set_value("address", _v_array(address_elements));
    if (shared_code.size() > 0) address.set_value("code", shared_code);
    return nelement;
  }

  /// @brief 住所文字列をジオコーディングする
  /// @arg   address  ジオコーディング結果を格納する Address オブジェクト
  /// @arg   it_s     住所の可能性のある Node 配列の先頭を指すイテレータ
  /// @arg   it_e     住所の可能性のある Node 配列の最後尾を指すイテレータ
  /// 住所として結果を出力する場合、
  ///  (1) address["address"] に住所要素のハッシュをセットし、
  ///  (2) it_s を必要な分進め、
  ///  (3) true を返す
  bool Service::tryDams(std::vector<geonlp::Address>& addresses, std::vector<Node>::iterator& it_s, const std::vector<Node>::iterator &it_e) const
    throw (damswrapper::DamsException) {
    std::vector<Node>::iterator it;
    std::string surface("");
    for (it = it_s; it != it_e;) {
      surface += (*it).get_surface();
      it++;
      if (surface.length() > 90) break; // UTF-8 で 30 文字まで
    }
    
    int score;
    std::string tail("--");
    std::vector<damswrapper::Candidate> candidates;

    while (tail.length() > 0) {
      std::cerr << "retrieve::surface:'" << surface << "'\n";
      damswrapper::retrieve(score, tail, candidates, surface);

      if (score < 4) return false; // 二階層以上が一致する候補なし

      int len = surface.length() - tail.length();
      surface = "";
      for (it = it_s; it != it_e;) {
	std::string next_surface((*it).get_surface());
	if (surface.length() + next_surface.length() == len) {
	  // ちょうど形態素の区切りまで住所として解析できた
	  surface += next_surface;
	  tail = "";
	  break;
	} else if (surface.length() + next_surface.length() > len) {
	  // 区切りが一致しなかった
	  break;
	}
	surface += next_surface;
	it++;
      }
    }

    geonlp::Address address;
    addresses.clear();
    for (std::vector<damswrapper::Candidate>::iterator it = candidates.begin(); it != candidates.end(); it++) {
      // damswrapper::Candidate& candidate = candidates[0]; // ToDo: 最適な candidate をコンテキストから選択する
      this->damsCandidateToAddress(address, surface, (*it));  // Address に積み替え
      addresses.push_back(address);
    }
    it_s = it;
    return true;
  }

  /// @brief  住所をジオコーディングする
  /// @arg @c address_string 住所文字列
  /// @return Address オブジェクト
  picojson::value Service::addressGeocoding(const picojson::array& params)
    throw (ServiceRequestFormatException) {
    picojson::value result;

    // パラメータ数チェック
    if (params.size() == 1) {
      ;
    } else if (params.size() == 2) {
      // オプションセット
      this->set_options(params[1]);
    } else {
      throw ServiceRequestFormatException("geonlp.addressGeocoding accepts 1 parameter.");
    }

    Address address;
    int score;
    std::string surface(""), tail("");
    std::vector<damswrapper::Candidate> candidates;

    // パラメータ解析
    if (params[0].is<std::string>()) {
      // １つの住所文字列のジオコーディング処理
      std::string address_string = params[0].get<std::string>();
      damswrapper::retrieve(score, tail, candidates, address_string);
      surface = address_string.substr(0, address_string.length() - tail.length());
      if (score < 1) {
	address.set_surface(surface);
      } else {
	this->damsCandidateToAddress(address, surface, candidates[0]);
      }
      result = (picojson::value)address.getGeoObject();
    } else if (params[0].is<picojson::array>()) {
      // 複数の住所文字列のジオコーディング処理
      picojson::array rarray;
      picojson::array params0 = params[0].get<picojson::array>();
      for (std::vector<picojson::value>::iterator it = params0.begin(); it != params0.end(); it++) {
	if (!(*it).is<std::string>()) throw ServiceRequestFormatException();
	std::string& address_string = (*it).get<std::string>();
	damswrapper::retrieve(score, tail, candidates, address_string);
	surface = address_string.substr(0, address_string.length() - tail.length());
	address.clear();
	if (score < 1) {
	  address.set_surface(surface);
	} else {
	  this->damsCandidateToAddress(address, surface, candidates[0]);
	}
	rarray.push_back((picojson::value)address.getGeoObject());
      }
      result = _v_array(rarray);
    }
    return result;
  }

#endif /* HAVE_LIBDAMS */

}
