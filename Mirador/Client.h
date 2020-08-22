#pragma once

#include "common.h"
#include "NetworkHelper.h"
#include "Hook.h"

class Client
{
	public:
		Client(const std::string & ip, const int &port);
		~Client();
		bool IsConnected();
		void Connect();
		SOCKET getSocket();

		static void Handle(void *arg);
		static void RegisterHook(const std::vector<std::string>& hooks, Hook* cb);

	private:
		static bool _connected;
		WSADATA _ws_d;
		SOCKET _connection;
		SOCKADDR *_sin;
		HANDLE _hdl;

		static std::map<std::string, Hook*> _hooks;
};

