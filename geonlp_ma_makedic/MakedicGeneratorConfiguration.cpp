#include "MakedicGeneratorConfiguration.h"
#include <boost/algorithm/string.hpp>

MakedicGeneratorConfiguration::MakedicGeneratorConfiguration(const std::string& ini_file_path) {
	read_ini(ini_file_path, conf_ini_file);
}

std::string MakedicGeneratorConfiguration::get_mecab_dict_index_path() const {
	return conf_ini_file.get<std::string>("mecab_dict_index_path");
}

/*
std::string MakedicGeneratorConfiguration::get_sysdic_directory_path() const {
	return conf_ini_file.get<std::string>("sysdic_directory_path");
}
*/

bool MakedicGeneratorConfiguration::get_delete_temp_file_flag() const {
	return conf_ini_file.get<bool>("delete_temp_file_flag");
}

std::vector<std::string> MakedicGeneratorConfiguration::get_example_sentences() const {
	std::string temp = conf_ini_file.get<std::string>("example_sentences");
	std::vector<std::string> sentences;
	boost::split(sentences, temp, boost::is_any_of("|"));
	return sentences;
}
