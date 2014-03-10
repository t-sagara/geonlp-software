///
/// @file
/// @brief MakedicGenerator(.exe) のアプリケーションロジッククラス。
/// @author Picolab Co.ltd
///
/// Copyright (c)2010, NII
///

#ifndef _MAKEDIC_GENERATOR_APP_H
#define _MAKEDIC_GENERATOR_APP_H

//#define DEFAULT_INI_FILE_PATH "/usr/local/etc/MakedicGenerator.ini"
#define DEFAULT_INI_FILE_PATH "/usr/local/etc/geonlp_ma_makedic.rc"

class MakedicGeneratorApp {

private:

	/// @brief 使い方を表示します。
	void usage(const char* progname);

public:

	/// @brief アプリケーションを起動します。
	///
	/// @param [in] argc main関数の引数 argc と同等
	/// @param [in] argv main関数の引数 argv と同等
	/// @return
	int exec(int argc, const char **argv);

};

#endif
