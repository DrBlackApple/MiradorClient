#pragma once

#include "common.h"

/* global settinges, edit it in Settings.cpp*/
class Settings
{
	public:
		Settings() = delete;
	
		static int server_port;
		static std::string server_ip;
		static int timeout;

		static bool useR77;

		static bool use_keylogger;
		static std::string keylogger_dir;

		static std::string cmd_path;
};

