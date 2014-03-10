#include <ruby.h>
#include <stdio.h>
#include <iostream>
#include "../include/GeonlpMA.h"

// private methods
static VALUE stlstring2rbstring(const std::string& stlstring) {
  return rb_str_new(stlstring.c_str(), stlstring.length());
}

// convert Geoword object to ruby hash
static VALUE _geoword_to_ruby_hash(const geoword::Geoword* pGeoword) {
  VALUE rb_geoword = rb_hash_new();
  {
    rb_hash_aset(rb_geoword, rb_str_new2("id"), INT2FIX(pGeoword->get_id()));
    rb_hash_aset(rb_geoword, rb_str_new2("geoword"), stlstring2rbstring(pGeoword->get_geoword()));
    {
      std::vector<std::string> c_upper_geowords = pGeoword->get_upper_geowords();
      VALUE rb_upper_geowords = rb_ary_new2(c_upper_geowords.size());
      for (unsigned int i = 0; i < c_upper_geowords.size(); i++) {
	rb_ary_store(rb_upper_geowords, i, stlstring2rbstring(c_upper_geowords[i]));
      }
      rb_hash_aset(rb_geoword, rb_str_new2("upper_geowords"), rb_upper_geowords);
    }
    rb_hash_aset(rb_geoword, rb_str_new2("desinence"), stlstring2rbstring(pGeoword->get_desinence()));
    rb_hash_aset(rb_geoword, rb_str_new2("yomi"), stlstring2rbstring(pGeoword->get_yomi()));
    rb_hash_aset(rb_geoword, rb_str_new2("pron"), stlstring2rbstring(pGeoword->get_pron()));
    rb_hash_aset(rb_geoword, rb_str_new2("geoword_full"), stlstring2rbstring(pGeoword->get_geoword_full()));
    rb_hash_aset(rb_geoword, rb_str_new2("yomi_full"), stlstring2rbstring(pGeoword->get_yomi_full()));
    rb_hash_aset(rb_geoword, rb_str_new2("pron_full"), stlstring2rbstring(pGeoword->get_pron_full()));
    rb_hash_aset(rb_geoword, rb_str_new2("latitude"), rb_float_new(pGeoword->get_latitude()));
    rb_hash_aset(rb_geoword, rb_str_new2("longitude"), rb_float_new(pGeoword->get_longitude()));
    rb_hash_aset(rb_geoword, rb_str_new2("theme_id"), stlstring2rbstring(pGeoword->get_theme_id()));
    rb_hash_aset(rb_geoword, rb_str_new2("code"), stlstring2rbstring(pGeoword->get_code()));
    rb_hash_aset(rb_geoword, rb_str_new2("state_code"), stlstring2rbstring(pGeoword->get_state_code()));
    rb_hash_aset(rb_geoword, rb_str_new2("city_code"), stlstring2rbstring(pGeoword->get_city_code()));
    rb_hash_aset(rb_geoword, rb_str_new2("address"), stlstring2rbstring(pGeoword->get_address()));
    rb_hash_aset(rb_geoword, rb_str_new2("note"), stlstring2rbstring(pGeoword->get_note()));
  }
  return rb_geoword;
}

/**
   Wrapper for MA Shared Pointer
 */
static void wrap_ma_free(geoword::MAPtr* p) {
  ruby_xfree(p);
}

static VALUE wrap_ma_allocate(VALUE self) {
  void *p = ruby_xmalloc(sizeof(geoword::MAPtr));
  memset(p, 0, sizeof(geoword::MAPtr));
  return Data_Wrap_Struct(self, NULL, wrap_ma_free, p);
}

static VALUE wrap_ma_initialize(VALUE self, VALUE vprofname) {
  const char *profname;
  geoword::MAPtr *pService;

  Check_Type(vprofname, T_STRING);
  Check_SafeStr(vprofname);
  profname = STR2CSTR(vprofname);
  Data_Get_Struct(self, geoword::MAPtr, pService);
  geoword::MAPtr service = geoword::createMA(profname);
  *pService = service;
  return Qnil;
}

/**
    Class Methods
 */

// service.parse(text)
// @param text string
// @return string
static VALUE wrap_ma_parse(VALUE self, VALUE vstring) {
  const char *cstring;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vstring, T_STRING);
  cstring = STR2CSTR(vstring);
  std::string str = (*pServicePtr)->parse(cstring);
  return stlstring2rbstring(str);
}

// service.parseNode(text)
// @param text string
// @return array of Hash
static VALUE wrap_ma_parse_node(VALUE self, VALUE vstring) {
  const char *cstring;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vstring, T_STRING);
  cstring = STR2CSTR(vstring);
  std::vector<geoword::Node> nodes = (*pServicePtr)->parseNode(cstring);
  
  // prepare the array for return values
  int node_length = nodes.size();
  VALUE rb_nodes = rb_ary_new2(node_length);

  for (unsigned int i = 0; i < node_length; i++) {
    // copy node info into hash
    geoword::Node c_node = nodes[i];
    VALUE rb_node = rb_hash_new();
    {
      rb_hash_aset(rb_node, rb_str_new2("surface"), stlstring2rbstring(c_node.get_surface()));
      rb_hash_aset(rb_node, rb_str_new2("partOfSpeech"), stlstring2rbstring(c_node.get_partOfSpeech()));
      rb_hash_aset(rb_node, rb_str_new2("subclassification1"), stlstring2rbstring(c_node.get_subclassification1()));
      rb_hash_aset(rb_node, rb_str_new2("subclassification2"), stlstring2rbstring(c_node.get_subclassification2()));
      rb_hash_aset(rb_node, rb_str_new2("subclassification3"), stlstring2rbstring(c_node.get_subclassification3()));
      rb_hash_aset(rb_node, rb_str_new2("conjugatedForm"), stlstring2rbstring(c_node.get_conjugatedForm()));
      rb_hash_aset(rb_node, rb_str_new2("conjugationType"), stlstring2rbstring(c_node.get_conjugationType()));
      rb_hash_aset(rb_node, rb_str_new2("originalForm"), stlstring2rbstring(c_node.get_originalForm()));
      rb_hash_aset(rb_node, rb_str_new2("yomi"), stlstring2rbstring(c_node.get_yomi()));
      rb_hash_aset(rb_node, rb_str_new2("pronunciation"), stlstring2rbstring(c_node.get_pronunciation()));
    }
    // store node info into the array
    rb_ary_store(rb_nodes, i, rb_node);
  }

  return rb_nodes;
}

// service.getGeowordEntry(id)
// @param id int
// @return Geoword as Hash
static VALUE wrap_ma_get_geoword_entry(VALUE self, VALUE vid) {
  int geoid; //const char *cstring;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vid, T_FIXNUM);
  geoid = FIX2INT(vid);
  geoword::Geoword c_geoword = (*pServicePtr)->getGeowordEntry(geoid);

  // copy geoword info into hash
  VALUE rb_geoword;
  if (c_geoword.get_id() == 0) {
    rb_geoword = Qnil;
  } else {
    rb_geoword = _geoword_to_ruby_hash(&c_geoword);
  }
  return rb_geoword;
}

// service.getGeowordEntries(str)
// @param str string
// @return array of Geoword
static VALUE wrap_ma_get_geoword_entries(VALUE self, VALUE vstring) {
  const char *cstring;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vstring, T_STRING);
  cstring = STR2CSTR(vstring);
  std::map<ID, geoword::Geoword> geowords = (*pServicePtr)->getGeowordEntries(cstring);

  VALUE rb_geowords = rb_hash_new();
  for (std::map<ID, geoword::Geoword>::iterator it = geowords.begin(); it != geowords.end(); it++) {
    rb_hash_aset(rb_geowords, INT2FIX((*it).first), _geoword_to_ruby_hash(&((*it).second)));
  }
  return rb_geowords;
}

// service.getGeowordIdList(str)
// @param str string
// @return string
static VALUE wrap_ma_get_geoword_id_list(VALUE self, VALUE vstring) {
  const char *cstring;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vstring, T_STRING);
  cstring = STR2CSTR(vstring);
  std::string idlist = (*pServicePtr)->getGeowordIdList(cstring);

  return stlstring2rbstring(idlist);
}

// service.getGeowordEntriesByStateCodeFromThemes(str, theme_regexps)
// @param str string
// @param theme_regexps string
// @return array of Geoword
static VALUE wrap_ma_get_geoword_entries_by_state_code_from_themes(VALUE self, VALUE vstate_code, VALUE vtheme_regexps) {
  const char *cstate_code, *ctheme_regexps;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vstate_code, T_STRING);
  Check_Type(vtheme_regexps, T_STRING);
  cstate_code = STR2CSTR(vstate_code);
  ctheme_regexps = STR2CSTR(vtheme_regexps);
  std::map<ID, geoword::Geoword> geowords = (*pServicePtr)->getGeowordEntriesByStateCodeFromThemes(cstate_code, ctheme_regexps);

  VALUE rb_geowords = rb_hash_new();
  for (std::map<ID, geoword::Geoword>::iterator it = geowords.begin(); it != geowords.end(); it++) {
    rb_hash_aset(rb_geowords, INT2FIX((*it).first), _geoword_to_ruby_hash(&((*it).second)));
  }
  return rb_geowords;
}

// service.getGeowordEntriesByCityCodeFromThemes(str, theme_regexps)
// @param str string
// @param theme_regexps string
// @return array of Geoword
static VALUE wrap_ma_get_geoword_entries_by_city_code_from_themes(VALUE self, VALUE vcity_code, VALUE vtheme_regexps) {
  const char *ccity_code, *ctheme_regexps;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vcity_code, T_STRING);
  Check_Type(vtheme_regexps, T_STRING);
  ccity_code = STR2CSTR(vcity_code);
  ctheme_regexps = STR2CSTR(vtheme_regexps);
  std::map<ID, geoword::Geoword> geowords = (*pServicePtr)->getGeowordEntriesByCityCodeFromThemes(ccity_code, ctheme_regexps);

  VALUE rb_geowords = rb_hash_new();
  for (std::map<ID, geoword::Geoword>::iterator it = geowords.begin(); it != geowords.end(); it++) {
    rb_hash_aset(rb_geowords, INT2FIX((*it).first), _geoword_to_ruby_hash(&((*it).second)));
  }
  return rb_geowords;
}

// service.setCustomThemes(regexps)
// @param regexps string
// @return nil
static VALUE wrap_ma_set_custom_themes(VALUE self, VALUE vstring) {
  const char *cstring;
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);

  Check_Type(vstring, T_STRING);
  cstring = STR2CSTR(vstring);
  if (*cstring != '\0') {
    (*pServicePtr)->setCustomThemes(cstring);
  } else {
    (*pServicePtr)->resetCustomThemes();
  }
  return Qnil;
}

// service.resetCustomThemes()
// @return nil
static VALUE wrap_ma_reset_custom_themes(VALUE self) {
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);
  (*pServicePtr)->resetCustomThemes();
  return Qnil;
}

// service.getActiveThemeIds()
// @return array of active theme id(string)
static VALUE wrap_ma_get_active_theme_ids(VALUE self) {
  geoword::MAPtr *pServicePtr;
  Data_Get_Struct(self, geoword::MAPtr, pServicePtr);
  const std::vector<std::string> ids = (*pServicePtr)->getActiveThemeIds();
  VALUE rb_theme_ids = rb_ary_new2(ids.size());
  for (std::vector<std::string>::const_iterator it = ids.begin(); it != ids.end(); it++) {
    rb_ary_push(rb_theme_ids, stlstring2rbstring(*it));
  }
  return rb_theme_ids;
}

/* Initialize */
extern "C" void Init_GeonlpMA(void) {
  // define class "GeonlpMA"
  VALUE rb_cGeonlpMA = rb_define_class("GeonlpMA", rb_cObject);
  // define private initializer
  rb_define_private_method(rb_cGeonlpMA, "initialize", (VALUE(*)(...))wrap_ma_initialize, 1);
  rb_define_alloc_func(rb_cGeonlpMA, wrap_ma_allocate);

  // define public methods
  rb_define_method(rb_cGeonlpMA, "parse", (VALUE(*)(...))wrap_ma_parse, 1);
  rb_define_method(rb_cGeonlpMA, "parseNode", (VALUE(*)(...))wrap_ma_parse_node, 1);
  rb_define_method(rb_cGeonlpMA, "getGeowordEntry", (VALUE(*)(...))wrap_ma_get_geoword_entry, 1);
  rb_define_method(rb_cGeonlpMA, "getGeowordEntries", (VALUE(*)(...))wrap_ma_get_geoword_entries, 1);
  rb_define_method(rb_cGeonlpMA, "getGeowordIdList", (VALUE(*)(...))wrap_ma_get_geoword_id_list, 1);
  rb_define_method(rb_cGeonlpMA, "getGeowordEntriesByStateCodeFromThemes", (VALUE(*)(...))wrap_ma_get_geoword_entries_by_state_code_from_themes, 2);
  rb_define_method(rb_cGeonlpMA, "getGeowordEntriesByCityCodeFromThemes", (VALUE(*)(...))wrap_ma_get_geoword_entries_by_city_code_from_themes, 2);
  rb_define_method(rb_cGeonlpMA, "setCustomThemes", (VALUE(*)(...))wrap_ma_set_custom_themes, 1);
  rb_define_method(rb_cGeonlpMA, "resetCustomThemes", (VALUE(*)(...))wrap_ma_reset_custom_themes, 0);
  rb_define_method(rb_cGeonlpMA, "getActiveThemeIds", (VALUE(*)(...))wrap_ma_get_active_theme_ids, 0);

  // define constant values
  //rb_define_const(rb_cGeonlpMA, "OK", INT2FIX(0));
}
