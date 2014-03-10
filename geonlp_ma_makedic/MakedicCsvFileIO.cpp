#include "MakedicCsvFileIO.h"
#include <fstream>

void MakedicCsvFileIO::write(const std::string& path, const std::vector<MakedicItem>& contents) {

	std::ofstream stream(path.c_str());

	for (std::vector<MakedicItem>::const_iterator it = contents.begin(); it != contents.end(); it++) {
		MakedicItem item = *it;
		stream << item.get_surface() << ","
				<< item.get_leftContextId() << ","
				<< item.get_rightContextId() << ","
				<< item.get_cost() << ","
				<< item.get_partOfSpeech() << ","
				<< item.get_subclassification1() << ","
				<< item.get_subclassification2() << ","
				<< item.get_subclassification3() << ","
				<< item.get_conjugatedForm() << ","
				<< item.get_conjugationType() << ","
				<< item.get_originalForm() << ","
				<< item.get_yomi() << ","
				<< item.get_pronunciation()
				<< std::endl;
	}

	stream.close();

	return;
}
