/*
 * GeonlpMA のユニットテスト
 */

#include <iostream>
#include <sstream>
#include "GeonlpMA.h"

void show_dics(geonlp::MAPtr ma) {
  const std::map<int, geonlp::Dictionary>& dics = ma->getActiveDictionaries();
  std::cout << "Active Dictionary" << std::endl;
  for (std::map<int, geonlp::Dictionary>::const_iterator it = dics.begin(); it != dics.end(); it++) {
    std::cout << (*it).first << "\t" << (*it).second.get_title() << std::endl;
  }
  std::cout << "-----------------" << std::endl;
  const std::vector<std::string>& classes = ma->getActiveClasses();
  std::cout << "Active Classes" << std::endl;
  for (std::vector<std::string>::const_iterator it = classes.begin(); it != classes.end(); it++) {
    std::cout << (*it) << std::endl;
  }
  std::cout << "-----------------" << std::endl;
  
}

void show_parse_result(geonlp::MAPtr ma, std::vector<geonlp::Node>& nodes) {
  for (unsigned int i = 0; i < nodes.size(); i++) {
    geonlp::Node c_node = nodes[i];
    std::cout << i
	      << "\tsurface:" << c_node.get_surface()
	      << "\tpartOfSpeech:" << c_node.get_partOfSpeech()
	      << std::endl;
    std::map<std::string, geonlp::Geoword> geowords;
    ma->getGeowordEntries(c_node, geowords);
    if (geowords.size() > 0) {
      for (std::map<std::string, geonlp::Geoword>::iterator it = geowords.begin(); it != geowords.end(); it++) {
	std::cout << (*it).first << ":" << (*it).second.getGeoJson() << std::endl;
      }
    }
  }
}

int main(int argc, char** argv) {
  std::string query("東京都、千代田区、多摩市、一ツ橋２－１－２、神保町駅");
  std::string res;
  geonlp::MAPtr ma = geonlp::createMA(); //"dba.rc");
  std::vector<int> new_dics;
  std::vector<std::string> new_classes;

  show_dics(ma);
  std::cout << "set 1, 2, 41" << std::endl;
  new_dics.clear(); new_dics.push_back(1); new_dics.push_back(2); new_dics.push_back(41);
  ma->setActiveDictionaries(new_dics);
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;

  std::cout << "reset" << std::endl;
  ma->resetActiveDictionaries();
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;

  std::cout << "add 1" << std::endl;
  new_dics.clear(); new_dics.push_back(1);
  ma->addActiveDictionaries(new_dics);
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;

  std::cout << "remove 41" << std::endl;
  new_dics.clear(); new_dics.push_back(41);
  ma->removeActiveDictionaries(new_dics);
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;

  std::cout << "reset" << std::endl;
  ma->resetActiveDictionaries();
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;
 
  std::cout << "add SubwayStation.*" << std::endl;
  new_classes.clear(); new_classes.push_back("SubwayStation.*");
  ma->addActiveClasses(new_classes);
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;

  std::cout << "set class to [] " << std::endl;
  new_classes.clear();
  ma->setActiveClasses(new_classes);
  show_dics(ma);
  res = ma->parse(query);
  std::cout << res << std::endl;

  new_dics.clear();
  new_classes.clear();
  ma->setActiveDictionaries(new_dics);
  ma->setActiveClasses(new_classes);
  std::cout << "get entries for \"あかさかみつけ\"" << std::endl;
  std::map<std::string, geonlp::Geoword> entries;
  ma->getGeowordEntries("あかさかみつけ", entries);
  for (std::map<std::string, geonlp::Geoword>::iterator it = entries.begin(); it != entries.end(); it++) {
    std::cout << (*it).second.toJson() << std::endl;
  }

  /* uncomment to check in interactive way
  for (;;) {
    std::string line;
    std::getline( std::cin, line);
    if (line.length() == 0) break;

    // parse
    std::string str = ma->parse( line);
    std::cout << str << std::endl;

    // parseNode
    std::vector<geonlp::Node> nodes;
    show_parse_result(ma, nodes);
  }
  */
  
}
